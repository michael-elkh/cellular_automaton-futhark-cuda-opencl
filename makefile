cuda:
	futhark cuda ac.fut
	./ac
opencl:
	futhark opencl ac.fut
	./ac
python:
	futhark python ac.fut
	./ac
c:
	futhark c ac.fut
	./ac