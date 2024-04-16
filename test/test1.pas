begin
  integer k;
  integer function F(n);
    begin
      integer n;
      if n<=0 then F:=1
      else F:=n*F(n-1);
      if n>=1 then F:=0
      else ;
    end;
  read(m);
  k:=F(m);
  write(k);
end