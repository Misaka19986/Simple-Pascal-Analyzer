begin
  integer k;
  integer funtion F(integer n);
    begin
      if n<=0 then F:=1
      else F:=n*F(n-1);
    end;
  read();
  k:=F(m);
  write(k);
end