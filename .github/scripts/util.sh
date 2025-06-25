
. .github/scripts/conf.sh

clone_source() {
    local pkgname="$1"
    local url="$2"
    local ref="$3"

    if [ ! -f $pkgname/.git/config ]; then
        echo "need to clone $pkgname"
        git clone $url $pkgname --branch=$ref --depth 1
    else
        echo "already cloned $pkgname"
    fi
}

build_meson() {
    local pkgname="$1"
    local url="$2"
    local ref="$3"
    shift
    shift
    shift
    if [ -f $X11_PREFIX/$pkgname.DONE ]; then
        echo "package $pkgname already built"
    else
        clone_source "$pkgname" "$url" "$ref"
        (
            cd $pkgname
            meson "$@" build -Dprefix=$X11_PREFIX
            ninja -j${FDO_CI_CONCURRENT:-4} -C build install
        )
        touch $X11_PREFIX/$pkgname.DONE
    fi
}

build_ac() {
    local pkgname="$1"
    local url="$2"
    local ref="$3"
    shift
    shift
    shift
    if [ -f $X11_PREFIX/$pkgname.DONE ]; then
        echo "package $pkgname already built"
    else
        clone_source "$pkgname" "$url" "$ref"
        (
            cd $pkgname
            ./autogen.sh --prefix=$X11_PREFIX
            make -j${FDO_CI_CONCURRENT:-4} install
        )
        touch $X11_PREFIX/$pkgname.DONE
    fi
}

build_drv_ac() {
    local pkgname="$1"
    local url="$2"
    local ref="$3"
    shift
    shift
    shift
    clone_source "$pkgname" "$url" "$ref"
    (
        cd $pkgname
        ./autogen.sh # --prefix=$X11_PREFIX
        make -j${FDO_CI_CONCURRENT:-4} # install
    )
}

build_ac_xts() {
    local pkgname="$1"
    local url="$2"
    local ref="$3"
    shift
    shift
    shift
    if [ -f $X11_PREFIX/$pkgname.DONE ]; then
        echo "package $pkgname already built"
    else
        clone_source "$pkgname" "$url" "$ref"
        (
            cd $pkgname
            CFLAGS='-fcommon'
            if [ "$X11_OS" = "Darwin" ]; then
                sed -E -i~ 's|(\[XTS\], \[)|\1xt xmu xaw7 |' configure.ac
                sed -E -i~ -e 's|(XTextProperty)[[:space:]]+(text_prop_good)|\1 *\2|' -e 's|(style_good),[[:space:]]*&(text_prop_good)|\1,\2|' -e 's|text_prop_good\.|text_prop_good->|' xts5/Xlib14/X{mb,wc}TextListToTextProperty.m
                CFLAGS="$CFLAGS -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0"
                if cc -Werror=unknown-warning-option -Wincompatible-function-pointer-types -c -xc -o /dev/null /dev/null 2>/dev/null; then
                    CFLAGS="$CFLAGS -Wno-error=incompatible-function-pointer-types"
                fi
            fi
            ./autogen.sh --prefix=$X11_PREFIX CFLAGS="$CFLAGS"
            make -j${FDO_CI_CONCURRENT:-4} install
        )
        touch $X11_PREFIX/$pkgname.DONE
    fi
}
