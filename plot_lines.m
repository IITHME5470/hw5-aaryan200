clear
tid = 6385;
directories = {'serial_plots', 'par_2x2_plots', 'par_2x4_plots', 'par_4x4_plots'};  % Specify your directories as a cell array
legends = {'Serial', '2x2 processors', '2x4 processors', '4x4 processors'};      % Specify corresponding legend labels

% Create figure for midline profile
figure, clf
hold on

% Loop through each directory
for i = 1:length(directories)
    % Construct file path
    if strcmp(directories{i}, 'serial_plots')
        file_path = fullfile(directories{i}, sprintf('T_x_y_%06d.dat', tid));
    else
        file_path = fullfile(directories{i}, sprintf('T_x_y_par_%06d.dat', tid));
    end
    
    % Read data file
    a = dlmread(file_path);
    
    % Calculate grid size and extract coordinates
    n = sqrt(size(a, 1));
    x = a(1:n:n^2, 1);          % X coordinates
    T = reshape(a(:, 3), [n, n]);
    
    % Extract midline profile (handle both even/odd grid sizes)
    mid_index = round(n/2);
    Tmid = T(:, mid_index);
    
    % Plot profile
    plot(x, Tmid, '-', 'LineWidth', 2)
end

% Format plot
hold off
xlabel('x', 'FontSize', 14)
ylabel('T', 'FontSize', 14)
title(sprintf('Profile along mid-y at t = %06d', tid), 'FontSize', 14)
xlim([-0.05 1.05])
legend(legends, 'FontSize', 14, 'Location', 'best')
grid on

% Save figure
screen2jpeg(sprintf('line_midy_T_comparison_%04d.png', tid))