% David Taylor, 7/14/2020
% matlab solution = x
% custom solution = soln

clc, clear, close all
%%
global N

choice = 5;

if choice == 1
    a = [3,2,-1,10;
        -1,3,2,5;
        1,-1,-1,-1];
elseif choice == 2
    a = [1,-1,2,8;
        0,0,-1,-11;
        0,2,-1,-3];
elseif choice == 3
    a = [0.003,59.14,59.17;
        5.291,-6.13,46.78];
elseif choice == 4
    a = [1,1,2,-4,-5,4,0;
        0,1,3,5,3,-2,1;
        3,5,-3,3,5,-5,2;
        2,-4,-3,1,0,-1,-5;
        5,-2,5,-2,2,-3,-4;
        2,-2,5,3,1,-1,2];
elseif choice == 5
    a = [1,1,1,0,1;
        1,1,1,1,0;
        1,0,1,1,1;
        0,0,0,1,0];
else
    error('invalid choice')
end

N = size(a,1);      % row size

temp = rref(a);     % temp is the reduced echelon form of a
x = temp(:,end);    

soln = solve_system(a);
numerr = compute_error(x,soln);

soln
numerr

function soln = solve_system(a) % takes in a
global N 

% Gaussian elimination
for i = 1:1:N                   % for each row
    
    % get pivot
    maxval = abs(a(i,i));       % pulls highest magnitude
    maxrow = i;                 
    for k = i+1:1:N
        temp = abs(a(k,i));
        if temp > maxval
            maxval = temp;
            maxrow = k;
        end
    end
    if maxval < 1e-6
        error('a is rank deficient')
    end                          % for now it pulls the largest value in each row
    
    % exchange rows
    for j = i:1:N+1
        temp = a(maxrow,j);
        a(maxrow,j) = a(i,j);
        a(i,j) = temp;
    end
    
    % get upper triangular form
    for k = i+1:1:N
        temp = -a(k,i)/a(i,i);
        for j = i:1:N+1
            if j == i
                a(k,j) = 0;
            else
                a(k,j) = a(k,j)+temp*a(i,j);
            end
        end
    end
    
end

% solve by back substitution
soln = NaN(N,1);
soln(N) = a(N,N+1)/a(N,N);
for i = N-1:-1:1
    temp = 0;
    for j = i+1:1:N
        temp = temp+a(i,j)*soln(j);
    end
    soln(i) = (a(i,N+1)-temp)/a(i,i);
end

end

function numerr = compute_error(x,soln)
global N

    % compute 2-norm of error
    temp = 0;
    for i = 1:N
        temp = temp+(x(i)-soln(i))*(x(i)-soln(i));
    end
    numerr = sqrt(temp);

end