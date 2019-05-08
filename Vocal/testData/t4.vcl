
-- This sample function demonstrates some of vocal syntax. Notice that
-- the language is very Ada-like. This is intentional. This file is not
-- exactly a careful test. In here I mostly just throw all sorts of
-- "normal" syntax at the parser to verify that it accepts it all
-- without complaint.
--
function sample_1() returns int is
	a:long;
	b:long;
	c:long;
	d:long;
begin
	a:=65535L + 1;
	b:=65536L - 0;
	c:=65535L + 1L;
        d:=65536L - 0L;

	return 0;
end
