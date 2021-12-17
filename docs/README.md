## Building the Docs

Install mkdocs:

	pip3 install mkdocs

Install mkdocs-material (3.3.0):

	pip3 install mkdocs-material==3.3.0

Install markdown-include

	pip3 install markdown-include

Build:

	cd docs
	mkdocs build

Test:

	mkdocs serve

Deploy:

	mkdocs gh-deploy