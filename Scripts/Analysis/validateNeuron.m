
function validateNeuron(file, networkDimensions, region, depth, row, col)

    if nargin > 2 && (region < 1 || region > length(networkDimensions)),
        error([file ' error: region ' num2str(region) ' does not exist'])
    elseif nargin > 3 && (depth < 1 || depth > networkDimensions(region).depth),
        error([file ' error: depth ' num2str(depth) ' does not exist'])
    elseif nargin > 4 && (min(row) < 1 || max(row) > networkDimensions(region).dimension),
        error([file ' error: row ' num2str(row) ' does not exist'])
    elseif nargin > 5 && (min(col) < 1 || max(col) > networkDimensions(region).dimension),
        error([file ' error: col ' num2str(col) ' does not exist'])
    end