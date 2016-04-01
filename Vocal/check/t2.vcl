
-- This sample function demonstrates some of vocal syntax. Notice that
-- the language is very Ada-like. This is intentional. This file is not
-- exactly a careful test. In here I mostly just throw all sorts of
-- "normal" syntax at the parser to verify that it accepts it all
-- without complaint.
--
function sample_1() returns int is
  a: long;

begin
	a := 65536L;
	return 0;
end
