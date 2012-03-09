
function MouseTest()

subplot(2,1,1);
imagesc(rand(5));

ax = subplot(2,1,2);
t= imagesc(rand(10));

set(t,'ButtonDownFcn',@mytestcallback) % WindowButtonDownFcn

function mytestcallback(hObject,~)
    pos=get(ax,'CurrentPoint');
    row = rround(pos(1, 2));
    col = rround(pos(1, 1));
    
    disp(['You clicked X:' num2str(col) ', Y:', num2str(row)]);
    
    function [res] = rround(i)
        
        if i < 0.5
            res = 1;
        else
            res = round(i);
        end
    end
end

end