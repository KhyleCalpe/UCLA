function y = controller(refDiff)
    if refDiff < 0
        y = 0.9;
    elseif refDiff < .03
        y = 0.8;
    elseif refDiff < .05
        y = 0.7;
    elseif refDiff < .8
        y = 0.5;
    elseif refDiff < .9
        y = 0.3;    
    else
        y = 0.1;
    end
end