/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#ifndef JLE_SKYBOX_H
#define JLE_SKYBOX_H

#include "jleCommon.h"

#include "jleSerializedResource.h"
#include "jleTypeReflectionUtils.h"

#include "jleResourceRef.h"
#include "jleImageFlipped.h"


class jleSkybox : public jleSerializedResource
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleSkybox, {"skyb"})

    SAVE_SHARED_THIS_SERIALIZED_JSON(jleSerializedResource)

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_right),
           CEREAL_NVP(_left),
           CEREAL_NVP(_bottom),
           CEREAL_NVP(_top),
           CEREAL_NVP(_front),
           CEREAL_NVP(_back));
    }

    ~jleSkybox() override = default;

    jleSkybox() = default;

    [[nodiscard]] bool loadFromFile(const jlePath &path) override;

    jleResourceRef<jleImageFlipped> _right;
    jleResourceRef<jleImageFlipped> _left;
    jleResourceRef<jleImageFlipped> _bottom;
    jleResourceRef<jleImageFlipped> _top;
    jleResourceRef<jleImageFlipped> _front;
    jleResourceRef<jleImageFlipped> _back;

    unsigned int getTextureID();

    unsigned int getVAO();

protected:
    unsigned int _vao{}, _vbo{}, _textureID{};
};

CEREAL_REGISTER_TYPE(jleSkybox)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedResource, jleSkybox)

#endif // JLE_SKYBOX_H