
function validateHistory(file, historyDimensions, objects, transforms, epochs, ticks)

    if nargin > 2 && (min(objects) < 1 || max(objects) > historyDimensions.numObjects),
        error([file ' error: object ' num2str(objects) ' does not exist'])
    elseif nargin > 3 && (min(transforms) < 1 || max(transforms) > historyDimensions.numTransforms),
        error([file ' error: transform ' num2str(transforms) ' does not exist'])
    elseif nargin > 4 && (min(epochs) < 1 || max(epochs) > historyDimensions.numEpochs),
        error([file ' error: epoch ' num2str(epochs) ' does not exist'])
    elseif nargin > 5 && (min(ticks) < 1 || max(ticks) > historyDimensions.numOutputsPrTransform),
        error([file ' error: tick ' num2str(ticks) ' does not exist'])
    end