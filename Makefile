all:
	@$(MAKE) -C src/

gen:
	@echo '     GEN   README.md';./src/generate-readme < book/book.md > README.md
	@./book/gen-html

clean:
	@$(MAKE) -C src $@
