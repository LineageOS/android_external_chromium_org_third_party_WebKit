/*
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SVGAnimatedBoolean_h
#define SVGAnimatedBoolean_h

#if ENABLE(SVG)
#include "core/svg/SVGAnimatedTypeAnimator.h"
#include "core/svg/properties/SVGAnimatedStaticPropertyTearOff.h"

namespace WebCore {

typedef SVGAnimatedStaticPropertyTearOff<bool> SVGAnimatedBoolean;

// Helper macros to declare/define a SVGAnimatedBoolean object
#define DECLARE_ANIMATED_BOOLEAN(UpperProperty, LowerProperty) \
DECLARE_ANIMATED_PROPERTY(SVGAnimatedBoolean, bool, UpperProperty, LowerProperty)

#define DEFINE_ANIMATED_BOOLEAN(OwnerType, DOMAttribute, UpperProperty, LowerProperty) \
DEFINE_ANIMATED_PROPERTY(AnimatedBoolean, OwnerType, DOMAttribute, DOMAttribute.localName(), UpperProperty, LowerProperty, SVGAnimatedBoolean, bool)

class SVGAnimationElement;

class SVGAnimatedBooleanAnimator : public SVGAnimatedTypeAnimator {
public:
    SVGAnimatedBooleanAnimator(SVGAnimationElement*, SVGElement*);
    virtual ~SVGAnimatedBooleanAnimator() { }
    
    virtual PassOwnPtr<SVGAnimatedType> constructFromString(const String&);
    virtual PassOwnPtr<SVGAnimatedType> startAnimValAnimation(const SVGElementAnimatedPropertyList&);
    virtual void stopAnimValAnimation(const SVGElementAnimatedPropertyList&);
    virtual void resetAnimValToBaseVal(const SVGElementAnimatedPropertyList&, SVGAnimatedType*);
    virtual void animValWillChange(const SVGElementAnimatedPropertyList&);
    virtual void animValDidChange(const SVGElementAnimatedPropertyList&);

    virtual void addAnimatedTypes(SVGAnimatedType*, SVGAnimatedType*);
    virtual void calculateAnimatedValue(float percentage, unsigned repeatCount, SVGAnimatedType*, SVGAnimatedType*, SVGAnimatedType*, SVGAnimatedType*);
    virtual float calculateDistance(const String& fromString, const String& toString);
};

} // namespace WebCore

#endif // ENABLE(SVG)
#endif