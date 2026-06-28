add_requires("enkits")
add_requires("vbase")

target("vtask")
	set_kind("static")

	add_headerfiles("include/(vtask/**.hpp)")
	add_includedirs("include", {public = true})

	add_files("src/**.cpp")

	add_packages("vbase", {public = true}) -- consumed from xmake-repo (was add_deps in workspace builds)

	add_packages("enkits") -- private dependency, not exposed to users

	-- set target directory
    set_targetdir("$(builddir)/$(plat)/$(arch)/$(mode)/vtask")