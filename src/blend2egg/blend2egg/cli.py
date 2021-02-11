import argparse
import os
import sys
import tempfile


from .common import Settings
from . import blenderutils


def convert(settings, srcdir, src, dst):
    if settings.pipeline == 'egg':
        from .converter.blend2egg_converter import ConverterBlend2Egg
        src2dst = ConverterBlend2Egg(settings)
        dstext = '.egg'
    else:
        raise RuntimeError('Unknown pipeline: {}'.format(settings.pipeline))

    for src_element in src:
        if not os.path.exists(src_element):
            print('Source ({}) does not exist'.format(src_element))
            sys.exit(1)

        if len(src) > 1 and not os.path.isfile(src_element):
            print('Source ({}) is not a file'.format(src_element))
            sys.exit(1)

        if len(src) == 1 and not (os.path.isfile(src_element) or os.path.isdir(src_element)):
            print('Source ({}) must be a file or a directory'.format(src))
            sys.exit(1)

    src_is_dir = os.path.isdir(src[0])
    dst_is_dir = not os.path.splitext(dst)[1]
    print(dst_is_dir,os.path.splitext(dst)[1])

    if dst_is_dir and not dst.endswith(os.sep):
        dst = dst + os.sep

    print('src', src, 'dst', dst)

    files_to_convert = []
    if src_is_dir:
        srcdir = src[0]
        for root, _, files in os.walk(srcdir):
            files_to_convert += [
                os.path.join(root, i)
                for i in files
                if i.endswith('.blend')
            ]
    else:
        files_to_convert = [os.path.abspath(i) for i in src]

    is_batch = len(files_to_convert) > 1 or dst_is_dir

    if is_batch and not dst_is_dir:
        print('Destination must be a directory if the source is a directory or multiple files')

    try:
        if is_batch:
            # Batch conversion
            dstfiles = [
                i.replace(srcdir, dst).replace('.blend', dstext)
                for i in files_to_convert
            ]
            src2dst.convert_batch(srcdir, dst, dstfiles)
        else:
            # Single file conversion
            srcfile = files_to_convert[0]
            if dst_is_dir:
                # Destination is a directory, add a filename
                dst = os.path.join(dst, os.path.basename(srcfile.replace('blend', 'egg')))

            src2dst.convert_single(srcfile, dst)
    except Exception: #pylint: disable=broad-except
        import traceback
        print(traceback.format_exc(), file=sys.stderr)
        print('Failed to convert all files', file=sys.stderr)
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(
        description='CLI tool to convert Blender blend files to Panda3D Egg files'
    )

    parser.add_argument('src', nargs='+', type=str, help='source path')
    parser.add_argument('dst', type=str, help='destination path')

    parser.add_argument(
        '--physics-engine',
        choices=[
            'builtin',
            'bullet',
        ],
        default='builtin',
        help='the physics engine to build collision solids for'
    )

    parser.add_argument(
        '--srcdir',
        default=None,
        help='a common source directory to use when specifying multiple source files'
    )

    parser.add_argument(
        '--blender-dir',
        default='',
        help='directory that contains the blender binary'
    )

    parser.add_argument(
        '--append-ext',
        action='store_true',
        help='append extension on the destination instead of replacing it (batch mode only)'
    )

    parser.add_argument(
        '--pipeline',
        choices=[
            'egg',
        ],
        default='egg',
        help='the backend pipeline used to convert files'
    )

    parser.add_argument(
        '--sf',
        default=-1,
        help='start frame of animation.  -1 to use the default'
    )

    parser.add_argument(
        '--ef',
        default=-1,
        help='end frame of animation.  -1 to use the default'
    )

    parser.add_argument(
        '--fps',
        default=-1,
        help='frame rate of animation.  -1 to use the default'
    )

    parser.add_argument(
        '--cn',
        default=None,
        help='explicit name to give the character/anim bundle'
    )

    parser.add_argument(
        '--ac',
        choices=[
            'none',
            'model',
            'chan'
        ],
        default='none',
        help='animation conversion type (none, model, or chan)'
    )

    parser.add_argument(
        '--cs',
        choices=[
            'y',
            'z'
        ],
        default='z',
        help='coordinate system up-axis of the model'
    )

    args = parser.parse_args()

    if args.srcdir:
        args.srcdir = args.srcdir.strip('"')
    if args.blender_dir:
        args.blender_dir = args.blender_dir.strip('"')

    src = [os.path.abspath(i.strip('"')) for i in args.src]

    if args.srcdir:
        srcdir = args.srcdir
    else:
        srcdir = os.path.dirname(src[0]) if len(src) == 1 else os.path.commonpath(src)
    dst = os.path.abspath(args.dst.strip('"'))

    if not args.blender_dir and not blenderutils.blender_exists():
        args.blender_dir = blenderutils.locate_blenderdir()
        if args.blender_dir:
            print('Auto-detected Blender installed at {}'.format(args.blender_dir))

    if not blenderutils.blender_exists(args.blender_dir):
        print(
            'Blender not found! Try adding Blender to the system PATH or using '
            '--blender-dir to point to its location',
            file=sys.stderr
        )
        sys.exit(1)

    settings = Settings(
        physics_engine=args.physics_engine,
        blender_dir=args.blender_dir,
        append_ext=args.append_ext,
        pipeline=args.pipeline,
        start_frame=args.sf,
        end_frame=args.ef,
        fps=args.fps,
        char_name=args.cn,
        anim_type=args.ac,
        coordinate_system=args.cs
    )

    convert(settings, srcdir, src, dst)
