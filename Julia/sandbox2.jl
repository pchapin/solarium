
# Call the rand function on process #2, passing 3, 3 as arguments.
r = remotecall(rand, 2, 3, 3)

# Spawn on "any" process the evaluation of the expression 1 .+ fetch(r). The @spawnat macro
# will be the process intelligently. In this case it will use process #2 making the fetch a nop.
s = @spawnat :any 1 .+ fetch(r)

# Display the result
print(fetch(s))

