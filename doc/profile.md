修改Profile之后如何启动

```
./lightguider -l test.phd
```

这里需要是绝对目录，不然会出错，加载完成之后Guider会退出，这个时候需要重新启动，启动之后配置就会生效。

也可以直接修改Sodium.apt，可以修改的参数更加的详细，具体看pdf

```

name=test # 配置文件名

[profile/2/camera]
binning=1
LastMenuchoice=INDI Camera [Guide Simulator]
pixelsize=2.4
gain=95

[profile/2/scope]
LastMenuChoice=INDI Mount [Telescope Simulator]

[profile/2/indi]
INDIhost=localhost
INDIport=7624
INDIcam=Guide Simulator
INDIcam_ccd=0
INDIcam_forcevideo=0
INDIcam_forceexposure=0
VerboseLogging=1
INDImount=Telescope Simulator

```