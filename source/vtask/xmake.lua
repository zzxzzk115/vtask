add_requires("enkits")

target("vtask")
	set_kind("static")

	add_headerfiles("include/(vtask/**.hpp)")
	add_includedirs("include", {public = true})

	add_files("src/**.cpp")

	add_deps("vbase", {public = true})

	add_packages("enkits") -- private dependency, not exposed to users

	-- set target directory
    set_targetdir("$(builddir)/$(plat)/$(arch)/$(mode)/vtask")