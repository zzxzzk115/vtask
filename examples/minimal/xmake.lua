target("vtask_example_minimal")
	set_kind("binary")
	add_files("main.cpp")
	add_deps("vtask")
	-- set target directory
	set_targetdir("$(builddir)/$(plat)/$(arch)/$(mode)/vtask_example_minimal")