

objects = check.o script.o update_uboot.o


edit:$(objects)
	gcc -o update_uboot $(objects)
	rm -rf $(objects)


clean:
	rm -rf update_uboot $(objects)
