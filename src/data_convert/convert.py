from xml.etree import ElementTree as ET  
import sys

def rparse(root):
	print root.tag
	size = len(root.getchildren())
	print size
	if size > 0:
		for i in root.getchildren():
			rparse(i)
	else:
		print root.text

if __name__ == '__main__':
	tree = ET.parse(sys.argv[1])
	root = tree.getroot()
	rparse(root)
