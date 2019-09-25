clear, clc

% Get timestamps for all measurements
filePath = '/home/achariso/Desktop/Embedded Systems/Projects/1/project1/cmake-build-debug/';
[timestampData, n] = read_data( [filePath 'TimestampsArray.txt'] );
[samplingPeriodData, ~] = read_data( [filePath 'SamplingPeriodsArray.txt'] );

% Plot
for mi = 1 : n
    
    figure('NumberTitle', 'off', 'Name', [...
        'Measurement #' num2str( mi ) ...
        ' ( ' ...
            num2str( timestampData.(['m' num2str( mi ) ]).actualNumberOfSamples ) ...
        '/' ...
            num2str( timestampData.(['m' num2str( mi ) ]).numberOfSamples ) ...
        ' samples using ' ...
            char( timestampData.(['m' num2str( mi ) ]).samplingFunction ) ...
        ' ) ' ...
    ])
    
    subplot( 2, 1, 1 )
    plot( timestampData.(['m' num2str( mi ) ]).data )
    title( 'Timestamps' )
    xlabel( 'sample index' )
    ylabel( 'timestamp ( sec )' )
    
    subplot( 2, 1, 2 )
    plot( samplingPeriodData.(['m' num2str( mi ) ]).data )
    title( 'Sampling Periods' )
    xlabel( 'sample index' )
    ylabel( 'period ( sec )' )
    
end
