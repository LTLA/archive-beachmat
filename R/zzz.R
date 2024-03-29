pkgconfig <- function(opt = c("PKG_LIBS", "PKG_CPPFLAGS"))
{
    path <- Sys.getenv(
        "RBEACHMAT_RPATH",
        system.file("lib", package="beachmat", mustWork=TRUE)
    )
    if (nzchar(.Platform$r_arch)) {
        arch <- sprintf("/%s", .Platform$r_arch)
    } else {
        arch <- ""
    }
    patharch <- paste0(path, arch)

    result <- switch(match.arg(opt), PKG_CPPFLAGS={
        sprintf('-I"%s"', system.file("include", package="beachmat"))
    }, PKG_LIBS={
        switch(Sys.info()['sysname'], Linux={
            sprintf('-L%s -Wl,-rpath,%s -lbeachmat -pthread', patharch, patharch)
        }, Darwin={
            sprintf('%s/libbeachmat.a %s -pthread', patharch, capture.output(Rhdf5lib::pkgconfig("PKG_CXX_LIBS")))
        }, Windows={
            ## for some reason double quotes aren't always sufficient
            ## so we use the 8+3 form of the path
            patharch <- gsub(x = utils::shortPathName(patharch),
                             pattern = "\\",
                             replacement = "/", 
                             fixed = TRUE)
            sprintf('-L%s -lbeachmat %s', patharch, capture.output(Rhdf5lib::pkgconfig("PKG_CXX_LIBS")))
        }
    )})

    cat(result)
}
