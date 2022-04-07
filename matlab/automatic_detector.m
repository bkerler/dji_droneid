clear all

%% Signal parameters
file_path = '/opt/dji/collects/5782GHz_30720KSPS.fc32';
sample_rate = 30.72e6;          % Collected 2x oversampled
critial_sample_rate = 15.36e6;  % The actual sample rate for this signal
carrier_spacing = 15e3;         % Per the LTE spec
signal_bandwidth = 10e6;        % Per the LTE spec
rough_frequency_offset = 5.5e6; % The collected signal is 5.5 MHz off center


%% Read in the IQ samples for a single burst
%  The burst is found using baudline and its cursor/delta time measurements
file_start_time = 6.507678;
file_start_sample_idx = uint64(file_start_time * sample_rate);

burst_duration_time = 0.000721;
burst_duration_samples = uint64(burst_duration_time * sample_rate);

samples = read_complex_floats(file_path, file_start_sample_idx, burst_duration_samples);

figure(1);
plot(10 * log10(abs(fftshift(fft(samples)))));
title('Original Samples')

%% Roughly center the signal of interest
%  The signal might have been collected at an offset, so remove that offset
rotation_vector = exp(1j * 2* pi / sample_rate * (0:length(samples)-1) * rough_frequency_offset);
samples = samples .* rotation_vector;

%% Interpolate the signal
%  The reason for interpolation is that it makes finding the correct
%  starting sample index more accurate.  The starting offset needs to be
%  very, very close so that pilots aren't needed.  A time offset results in
%  a walking phase offset after the FFT which is hard to remove without
%  pilots.  The higher the interpolation factor the better for getting the
%  correct offset, but that adds to computation time and memory use

% Keep in mind that the signal may already be interpolated (ie not
% critically sampled) at this point.  If the signal was recorded 2x
% oversampled, then an interpolation of 2 here means that the signal is 4x
% oversampled.
interpolation = 2;
interpolated_samples = zeros(1, length(samples) * interpolation);
interpolated_samples(1:interpolation:end) = samples;

% Define the interpolation filter.  The passband is 1/2 of the bandwidth
% (there are plenty of guard carriers to cover the stop-band).  Allowing
% for 400 KHz of stop-band.  Using 200 taps as that seems to work well
passband_edge = signal_bandwidth/2;
stopband_edge = signal_bandwidth/2 + 400e3;
filter_taps = fir1(200, signal_bandwidth/2/sample_rate*interpolation/2);
interpolated_samples = filter(filter_taps, 1, interpolated_samples);

% Calculate new rate dependant variables
interpolated_fft_size = sample_rate * interpolation / carrier_spacing;
interpolated_long_cp = round(1/192000 * sample_rate * interpolation);
interpolated_short_cp = round(0.0000046875 * sample_rate * interpolation);

figure(2);
plot(10 * log10(abs(fftshift(fft(interpolated_samples)))));
title('Interpolated Rate')

% No reason to search *all* of the samples.  The ZC sequence is the third
% OFDM symbol, so search for up to 5 OFDM symbols.  The first OFDM symbol
% has a long cyclic prefix, and the remaining 4 use the short sequence
search_window_length = (interpolated_long_cp + (interpolated_short_cp * 4)) + (interpolated_fft_size * 5);
[scores] = find_zc(interpolated_samples(1:search_window_length), sample_rate * interpolation);
[score, index] = max(abs(scores).^2);
if (score < 0.8)
    error('Failed to find the first ZC sequence');
end

% Calculate where in the interpolated samples the burst should start
% The calculation is just backing up by 2 short cyclic prefixed, 1 long
% cyclic prefix, and 3 FFT sizes
start_offset = index - (((interpolated_short_cp + interpolated_fft_size) * 2) + ...
    interpolated_long_cp + interpolated_fft_size);

% The signal is already filtered, so just throw away every N samples to
% decimate down to critical rate
decimation_rate = interpolation * (sample_rate / critial_sample_rate);
samples = interpolated_samples(start_offset:decimation_rate:end);

figure(3);
plot(10 * log10(abs(fftshift(fft(samples)))));
title('Critical Rate')

%% Calculate critical rate parameters
fft_size = critial_sample_rate / carrier_spacing;
long_cp_len = round(0.0000052 * critial_sample_rate);
short_cp_len = round(0.00000469 * critial_sample_rate);

true_start_index = round(start_offset/decimation_rate);

%% Plot symbol overlays
%  The logic below is for debugging only.  It draws OFDM symbol boundaries
%  over a time domain view of the burst.  It alternates between red and
%  green transparent rectangles over the time domain as a sanity check that
%  the time alignment is correct (starting at the right sample)
plot_symbol_boundaries(samples, critial_sample_rate, 4);

%% Coarse frequency adjustment
offset_freq = -.156e3;
offset_adj = offset_freq / critial_sample_rate;
% samples = samples .* exp(1j * 2 * pi * offset_adj * (0:length(samples)-1));

[offset_est] = estimate_cp_freq_offset(samples, fft_size, short_cp_len, long_cp_len);
offset_est * critial_sample_rate

% offset_freq = .156e3;
% offset_adj = offset_freq / critial_sample_rate;
samples = samples .* exp(1j * 2 * pi * offset_est * 9 * (0:length(samples)-1));

%% Symbol extraction

symbols_time_domain = zeros(9, fft_size);
symbols_freq_domain = zeros(9, fft_size);

sample_offset = 1;
for symbol_idx=1:9
    if (symbol_idx == 1 || symbol_idx == 9)
        cyclic_prefix_len = long_cp_len;
    else
        cyclic_prefix_len = short_cp_len;
    end
    
    sample_offset = sample_offset + cyclic_prefix_len;
    symbol = samples(sample_offset:sample_offset + fft_size - 1);
    symbols_time_domain(symbol_idx,:) = symbol;
    symbols_freq_domain(symbol_idx,:) = fftshift(fft(symbol));

    sample_offset = sample_offset + fft_size;
end

figure(5)
for idx=1:9
    subplot(3, 3, idx);
    plot(symbols_freq_domain(idx,:), 'o');
end
title('Constellations (Pre Freq Correction)')
