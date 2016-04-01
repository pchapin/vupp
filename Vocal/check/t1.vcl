
-- This sample function demonstrates some of vocal syntax. Notice that
-- the language is very Ada-like. This is intentional. This file is not
-- exactly a careful test. In here I mostly just throw all sorts of
-- "normal" syntax at the parser to verify that it accepts it all
-- without complaint.
--
function sample_1() returns int is
   a : int;
   b : int;
   c : int;
   d : int;
   e : int;
   e2 : int;
   f1 : int;
   f2 : int;
   g : int;
   h : int;
   i : int;
   j : int;
   k1 : int;
   k2 : int;
   l1 : int;
   l2 : int;
   mmm : int;
   n : int;
   o : int;
   p : int;
   q : int;
   r : int;
   s : int;
   t : int;
   u : int;
   v : int;
   w : int;
   x : int;
   y : int;
   z : int;

begin
	a := 2 == 2;
	b := 3 != 2;
	c := 2 < 3;
	d := 4 > 3;
	e := 2 <= 3;
	e2:= 2 <= 2;
	f1 := 4 >= 3;
	f2:= 3 >= 3;
	g  := 65537L == 65537L;
	h  := 67003L != 67002L;
	i  := 67002L < 67003L;
	j   := 67004L > 67003L;
	k1   := 67002L <= 67003L;
	k2  := 67003L <= 67003L;
	l1   := 67004L >= 67003L;
	l2  := 67003L >= 67003L;
	mmm := 65535; -- So I can debug easier
	n := 3 == 2;
	o := 2 != 2;
	p := 3 < 3;
	q := 3 > 3;
	r := 4 <= 3;
	s  := 2 >= 3;
	t  := 65534L == 65537L;
	u  := 67002L != 67002L;
	v  := 67003L < 67003L;
	w   := 67003L > 67003L;
	x    := 67004L <= 67003L;
	y    := 67002L >= 67003L;
        z := 65535;

	return 0;
end
