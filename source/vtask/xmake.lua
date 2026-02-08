add_requires("taskflow v3.11.0")

target("vtask")
	set_kind("static")

	add_headerfiles("include/(vtask/**.hpp)")
	add_includedirs("include", {public = true})

	add_files("src/**.cpp")

	add_packages("taskflow")

	-- set target directory
    set_targetdir("$(builddir)/$(plat)/$(arch)/$(mode)/vtask")