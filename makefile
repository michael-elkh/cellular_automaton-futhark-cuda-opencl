cuda:
	futhark cuda ac.fut
	@echo 1 | ./ac
opencl:
	futhark opencl ac.fut
	@echo 1 | ./ac
python:
	futhark python ac.fut
	@echo 1 | ./ac
c:
	futhark c ac.fut
	@echo 1 | ./ac

clean:
	rm -vf ac.py ac.c ac