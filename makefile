default: make1 make2 make3

make1:
	cd Project2adapted && $(MAKE)
make2:
	cd Webcrawler && $(MAKE)
make3:
	cd Webserver && $(MAKE)
