import os
import sys

import bpy #pylint: disable=import-error

sys.path.append(os.path.join(os.path.dirname(__file__), '..', ))
import blender_script_common as common #pylint: disable=import-error,wrong-import-position

def export_egg(settings, src, dst):
    print('Converting .blend file ({}) to .egg ({})'.format(src, dst))

    # Lazy-load yabee
    scriptdir = os.path.dirname(__file__)
    sys.path.insert(0, os.path.join(scriptdir, 'yabee'))
    sys.path.insert(0, os.path.join(scriptdir))
    import yabee #pylint: disable=import-error
    if not hasattr(bpy.context.scene, 'yabee_settings'):
        yabee.register()

    yabee_settings = bpy.context.scene.yabee_settings
    yabee_settings.opt_anim = [settings["start_frame"], settings["end_frame"], settings["fps"]]
    print("Anim options:", yabee_settings.opt_anim)
    yabee_settings.opt_use_loop_normals = True
    yabee_settings.opt_char_name = settings["char_name"]
    yabee_settings.opt_anim_type = settings["anim_type"]
    yabee_settings.opt_coordinate_system = settings["coordinate_system"]

    p3d_egg_export( #pylint: disable=undefined-variable
        dst,
        yabee_settings.opt_anim,
        yabee_settings.opt_anim_type,
        yabee_settings.opt_tbs_proc,
        yabee_settings.opt_autoselect,
        yabee_settings.opt_apply_object_transform,
        yabee_settings.opt_merge_actor,
        yabee_settings.opt_apply_modifiers,
        yabee_settings.opt_apply_collide_tag,
        yabee_settings.opt_use_loop_normals,
        yabee_settings.opt_force_export_vertex_colors,
        yabee_settings.opt_char_name,
        yabee_settings.opt_coordinate_system
    )


if __name__ == '__main__':
    common.convert_files(export_egg, 'egg')
