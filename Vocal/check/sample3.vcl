
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
  b : array of 10 long;
  x : int;
  y : int;
  z : long;

  begin
    x := 1;
    y := 2;

    z := x+2;

    a[z] := 37;

    b[x+2] := 37;

    return a[x+2];
  end
