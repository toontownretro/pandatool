from collections import namedtuple

Settings = namedtuple('Settings', (
    'physics_engine',
    'blender_dir',
    'append_ext',
    'pipeline',
    'start_frame',
    'end_frame',
    'fps',
    'char_name',
    'anim_type',
    'coordinate_system'
))
Settings.__new__.__defaults__ = (
    'builtin', # physics engine
    '', # blender_dir
    False, # append_ext
    'egg', # pipeline
    None, # animations
    -1, # start frame
    -1, # end frame
    -1, # fps
    'none', # anim_type,
    'z' # coordinate_system
)

class ConverterBase:
    '''Implements common functionality for converters'''

    def __init__(self, settings=None):
        if settings is None:
            settings = Settings()
        self.settings = settings

    def convert_single(self, src, dst):
        '''Convert a single src file to dst'''
        raise NotImplementedError()

    def convert_batch(self, srcroot, dstdir, files):
        '''Convert files from srcroot to dstdir'''
        raise NotImplementedError()
