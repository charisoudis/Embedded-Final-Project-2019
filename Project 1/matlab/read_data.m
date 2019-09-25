function [data, numberOfMeasurements] = read_data( file )

    fid = fopen( file );
    
    numberOfMeasurements = 0;
    data = struct;
    
    while 1
        % Read Line
        fline = fgetl( fid );
        if ( -1 == fline ), break, end
        
        % Check if new measurment
        if ( startsWith( fline, '--- New Sampling', 'IgnoreCase', false ) )
           
            measurementData = sscanf( fline, ...
                '--- New Sampling ( %d/%d samples - %f secs period - using %s ) ---' );
            
            numberOfMeasurements = numberOfMeasurements + 1;
            data.(['m' num2str( numberOfMeasurements ) ]) = struct( ...
                'numberOfSamples', 0, ...
                'actualNumberOfSamples', 0, ...
                'samplingPeriod', 0.0, ...
                'samplingFunction', '', ...
                'data', zeros( measurementData( 1 ), 1 ) ...
            );
        
            data.(['m' num2str( numberOfMeasurements ) ]).actualNumberOfSamples = ...
                measurementData( 1 );
            
            data.(['m' num2str( numberOfMeasurements ) ]).numberOfSamples = ...
                measurementData( 2 );
            
            data.(['m' num2str( numberOfMeasurements ) ]).samplingPeriod = ...
                measurementData( 3 );
            
            data.(['m' num2str( numberOfMeasurements ) ]).samplingFunction = ...
                convertCharsToStrings( char( measurementData( 4 : end - 2 ) ) );
            
            % Get Data            
            for i = 1 : measurementData( 1 )
                
                data.(['m' num2str( numberOfMeasurements ) ]).data( i ) = ...
                    str2double( fgetl( fid ) );
                
            end
            
        end
    end
    
    fclose( fid );

end

