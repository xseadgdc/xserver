
xorg_sdk_headers = [
    '../common/compiler.h',
    '../common/xf86.h',
    '../common/xf86Module.h',
    '../common/xf86Opt.h',
    '../common/xf86PciInfo.h',
    '../common/xf86Priv.h',
    '../common/xf86Privstr.h',
    '../common/xf86cmap.h',
    '../common/xf86fbman.h',
    '../common/xf86str.h',
    '../common/xf86Xinput.h',
    '../common/xisb.h',
    '../common/xorgVersion.h',
    '../common/xf86sbusBus.h',
    '../common/xf86VGAarbiter.h',
    '../common/xf86Optionstr.h',
    '../common/xf86platformBus.h',
    '../common/xf86MatchDrivers.h',
    '../common/xaarop.h',
]

if build_dga
    xorg_sdk_headers += '../common/dgaproc.h'
endif

if build_xv
    xorg_sdk_headers += ['../common/xf86xv.h', '../common/xf86xvmc.h', '../common/xf86xvpriv.h']
endif

if build_xorg_sdk
    install_data(xorg_sdk_headers, install_dir: xorgsdkdir)
endif
