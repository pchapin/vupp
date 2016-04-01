
-- This sample function demonstrates some of vocal syntax. Notice that
-- the language is very Ada-like. This is intentional. This file is not
-- exactly a careful test. In here I mostly just throw all sorts of
-- "normal" syntax at the parser to verify that it accepts it all
-- without complaint.
--
function sample_1(m : int; n : long) returns int is

  -- Only two types are currently supported: int and long. However,
  -- arrays are also supported (only one dimension).
  a : array of 10 int;
  x : int;
  y : int;
  z : long;

  begin
    x := 1;
    y := 2;

    -- The usual control structures with the usual relational operators.
    if x < y then
      x := z + 1;
      y := (x + y)/(x - y);
    end

    if a[x + 1] == y - 1 and x - 1 > 2*y then
      a[x + 1] := a[a[z]];
      y := 8;
    else
      x := 9;
      y := 10;
    end

    while x <= y or x < 5 loop
      x := 7;
      y := 8;
    end

    -- Of course there is a return statement.
    return x + a[y];
  end
