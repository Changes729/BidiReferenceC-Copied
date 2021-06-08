# simple c cpp project

## usage

- build / run
  keyboard `ctrl + shift + b` or `Terminal -> Run Task ...`

- debug
  keyboard `F5` or `Run -> Start Debugging`

## Structure

```vi
Project_Name
|-- .vscode
|-- build
|-- CMakeLists.txt
|-- dist
|-- doc
|-- include
|-- config
|-- LICENSE
|-- README.md
|-- resource
|-- script
|-- spike
|-- src
|-- test
|-- tools
```

- [build](#build): 存放 CMake 产生的中间文件。它不是必须的，完全可以用`tmp`目录替代，但用`build`来存放 CMake 中间文件，可以说是开发者们不约而同的。
- [dist](#dist): dist 多用在 Web 工程上，全称因该是`distribute`。目前知道的有些人会用`deploy`作为这个目录的替代。
- [doc](#doc): `document`, 并不是所有工程都会做好文档管理，也并不是所有文档都是简单的文本。但 doc 应该要充当书籍，充当整个工程历史的史官。
- [include](#include): 引入的头文件，外部引入的有**头文件**，**源代码**或者**编译库**。
- [spike](#spike): 类似于`playground`。
- [src](#src): 源代码。
- [test](#test): 测试工具。
- [tools](#tools): 构建工具。 ncurse 工程中有个 progs 目录，就类似这个。它里面都放着方便调试用的工具。
- [config](#config): `miscellaneous`,配置文件，杂项文件。ncurse 工程中的 misc 文件夹，就放了许多运行时需要配置文件。
