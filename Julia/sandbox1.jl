
size = 100

# Create a one dimensional array of double precision floating point values.
a = zeros(size)

# Execute a parallel loop over the array.
Threads.@threads for i = 1:size
    a[i] = Threads.threadid()
end

print(a)

