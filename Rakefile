require 'rake/clean'

COLLADA_LIBS = ['cmake-make/lib/libGeneratedSaxParser.a',
                'cmake-make/lib/libMathMLSolver.a',
                'cmake-make/lib/libOpenCOLLADABaseUtils.a',
                'cmake-make/lib/libOpenCOLLADAFramework.a',
                'cmake-make/lib/libOpenCOLLADASaxFrameworkLoader.a',
                'cmake-make/lib/libOpenCOLLADAStreamWriter.a',
                'cmake-make/lib/libUTF.a',
                'cmake-make/lib/libbuffer.a',
                'cmake-make/lib/libftoa.a',
               ]

# compiler flags
$C_FLAGS = ['-Wall', '-DQ_LITTLE_ENDIAN',
            "-I#{File.expand_path('src')}",
            "-I#{File.expand_path('abaci/src')}",
            "-I#{File.expand_path('opencollada/COLLADABaseUtils/include')}",
            "-I#{File.expand_path('opencollada/COLLADAFramework/include')}",
            "-I#{File.expand_path('opencollada/COLLADAStreamWriter/include')}",
            "-I#{File.expand_path('opencollada/COLLADASaxFrameworkLoader/include')}",
            `libpng-config --cflags`.chomp,
           ]

$DEBUG_C_FLAGS = ['-g',
                  '-DDEBUG',
                 ]

$OPT_C_FLAGS = ['-O3', '-DNDEBUG']

# linker flags
L_FLAGS = ['-lstdc++',
           "-L#{File.expand_path('cmake-make/lib')}",
           '-lGeneratedSaxParser',
           '-lMathMLSolver',
           '-lOpenCOLLADABaseUtils',
           '-lOpenCOLLADAFramework',
           '-lOpenCOLLADASaxFrameworkLoader',
           '-lOpenCOLLADAStreamWriter',
           '-lUTF',
           '-lbuffer',
           '-lftoa',
           '-lpcre',
           '-lxml2',
           `libpng-config --ldflags`.chomp,
          ]

OBJECTS = ['src/q3bsp2dae.o',
           'src/q3bsp.o',
           'src/quadpatch.o',
           'src/geometryexporter.o',
           'src/sceneexporter.o',
           'src/materialexporter.o',
           'src/effectexporter.o',
           'src/imageexporter.o',
          ]

DEPS = OBJECTS.map {|f| f[0..-3] + '.d'}
EXE = 'q3bsp2dae'

# Use the compiler to build makefile rules for us.
# This will list all of the pre-processor includes this source file depends on.
def make_deps t
  sh "gcc -MM -MF #{t.name} #{$C_FLAGS.join ' '} -c #{t.source}"
end

# Compile a single compilation unit into an object file
def compile obj, src
  sh "gcc #{$C_FLAGS.join ' '} -c #{src} -o #{obj}"
end

# Link all the object files to create the exe
def link exe, objects
  sh "gcc #{objects.join ' '} -o #{exe} #{L_FLAGS.join ' '}"
end

# generate makefile rules from source code
rule '.d' => '.cpp' do |t|
  make_deps t
end

COLLADA_LIBS.each do |lib|
  file lib do |t|
    build_opencollada
  end
end

def build_opencollada
  unless File.exist? "cmake-make"
    sh "mkdir cmake-make"
  end
  sh "cd cmake-make; cmake ../opencollada"
  sh "cd cmake-make; make"
end

desc 'Build opencollada'
task :opencollada do
  build_opencollada
end

# adds .o rules so that objects will be recompiled if any of the contributing source code has changed.
task :add_deps => DEPS do
  OBJECTS.each do |obj|
    dep = obj[0..-3] + '.d'
    raise "Could not find dep file for object #{obj}" unless dep

    # open up the .d file, which is a makefile rule (built by make_deps)
    deps = []
    File.open(dep, 'r') {|f| f.each {|line| deps |= line.split}}
    deps.reject! {|x| x == '\\'}  # remove '\\' entries

    # Add a new file rule which will build the object file from the source file.
    # Note: this object file depends on all the pre-processor includes as well
    file obj => deps[1,deps.size] do |t|
      compile t.name, t.prerequisites[0]
    end
  end
end

file EXE => [:add_deps] + OBJECTS + COLLADA_LIBS do
  link EXE, OBJECTS
end

task :build => EXE
task :add_opt_flags do
  $C_FLAGS += $OPT_C_FLAGS
end
task :add_debug_flags do
  $C_FLAGS += $DEBUG_C_FLAGS
end

desc "Optimized Build"
task :opt => [:add_opt_flags, EXE]

desc "Debug Build"
task :debug => [:add_debug_flags, EXE]

desc "Optimized Build, By Default"
task :default => [:opt]

CLEAN.include DEPS, OBJECTS
CLOBBER.include EXE, 'cmake-make'
