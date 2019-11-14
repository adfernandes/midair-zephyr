# -*- python -*-
#
# python misc/map.py ~/Developer/Toolchains/gcc-arm-none-eabi/bin/arm-none-eabi-nm $(find _build/Manual -iname "*.obj") > _midair.dot && dot _midair.dot -Tpdf -o _midair.pdf

import argparse
import io
import os
import subprocess


parser = argparse.ArgumentParser(description="Map which symbols go to and from which objects.")

parser.add_argument("nm", type=str, help="the path to the 'nm' command")
parser.add_argument("file", type=str, nargs='+', help="one or more object file names")

args = parser.parse_args()


class Object:

    count = 0

    def __init__(self, filename):

        self.name = os.path.basename(filename)
        nm = subprocess.Popen([args.nm, "--extern-only", "--demangle", filename], stdout=subprocess.PIPE)
        entries = [s.decode('utf8') for s in nm.stdout.readlines()]
        entries = [s.split()[-2:] for s in entries]

        self.exports = { entry[1] : None for entry in list(filter(lambda entry: entry[0] == 'T', entries))}
        self.imports = { entry[1] : None for entry in list(filter(lambda entry: entry[0] == 'U', entries))}

        for entry in self.exports.keys():
            Object.count = Object.count + 1
            self.exports[entry] = f"T{Object.count:06}"

        for entry in self.imports.keys():
            Object.count = Object.count + 1
            self.imports[entry] = f"U{Object.count:06}"

    def remove_non_imported(self, imports):
        exports = {}
        for symbol in self.exports.keys():
            if symbol in imports:
                exports[symbol] = self.exports[symbol]
        self.exports = exports

    def remove_non_exported(self, exports):
        imports = {}
        for symbol in self.imports.keys():
            if symbol in exports:
                imports[symbol] = self.imports[symbol]
        self.imports = imports

    def subgraph(self, output):

        Object.count = Object.count + 1
        cluster = f"Obj{Object.count:06}"

        if len(self.exports) == 0 and len(self.imports) == 0:
            return

        output.write(f'    subgraph cluster_{cluster} {{ label = <<B>{self.name}</B>>; penwidth = 2.0; \n')
        output.write(f'\n')

        if len(self.exports) > 0:
            output.write(f'        subgraph cluster_{cluster}_exports {{ label = <<I>exports</I>>; penwidth = 1.0;\n')
            for symbol, label in self.exports.items():
                output.write(f'            {label} [label = "{symbol}", penwidth = 0.5];\n')
            output.write(f'        }}\n')
            output.write(f'\n')

        if len(self.imports) > 0:
            output.write(f'        subgraph cluster_{cluster}_imports {{ label = <<I>imports</I>>; penwidth = 1.0;\n')
            for symbol, label in self.imports.items():
                output.write(f'            {label} [label = "{symbol}", penwidth = 0.5];\n')
                self.imports[symbol] = label
            output.write(f'        }}\n')
            output.write(f'\n')

        output.write(f'    }}\n')
        output.write(f'\n')

    @staticmethod
    def digraph(objects):

        export_map = {}
        exports = set()
        imports = set()

        for object in objects:
            export_map.update(object.exports)
            exports.update(object.exports.keys())
            imports.update(object.imports.keys())

        for object in objects:
            object.remove_non_imported(imports)
            object.remove_non_exported(exports)

        output = io.StringIO()

        output.write("digraph elf {\n")
        output.write("\n")
        output.write("    graph [fontname = \"Consolas\"];\n")
        output.write("     node [fontname = \"Consolas\"];\n")
        output.write("     edge [fontname = \"Consolas\"];\n")
        output.write("\n")

        for object in objects:
            object.subgraph(output)

        for object in objects:
            for symbol, label in object.imports.items():
                if symbol in exports:
                    output.write(f'    {label} -> {export_map[symbol]};\n')
        output.write("\n")

        output.write("}\n")

        return output.getvalue()


objects = [Object(filename) for filename in args.file]
digraph = Object.digraph(objects)

print(digraph, end='')
