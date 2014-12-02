import re
import sys

if __name__ == "__main__":
	p1 = re.compile('&.*;')
	p2 = re.compile('<sup>')
	p3 = re.compile('</sup>')
	p4 = re.compile('<sub>')
	p5 = re.compile('</sub>')
	p6 = re.compile('<i>')
	p7 = re.compile('</i>')
	for line in open(sys.argv[1], "r").readlines():
		s1 = p1.sub('', line)
		s2 = p2.sub('', s1)
		s3 = p3.sub('', s2)
		s4 = p4.sub('', s3)
		s5 = p5.sub('', s4)
		s6 = p6.sub('', s5)
		print p7.sub('', s6)