#ifndef _COMPONENT__H_
#define _COMPONENT__H_

/// COMPONENT Macro
//
// This is a special macro that is used for substitution from the
// Atome Heart Engine preprocessor. It does nothing by itself, but the
// preprocessor uses it to know what components are available and
// build the code needed for the components to work automatically.
//
// There are several options that can be passed to COMPONENT
//
/// POOLING
//
// pool=y|n (default value: y)
//
// if the preprocessor should create a component_pool for this
// component inside the entity_manager.
//
/// ACCESSORS
//
// accessors=y|n (default value: y)
//
// if the preprocessor should create accessors to get this component
// from an entity
//
// MANUAL MANAGEMENT
//
// manual
//
// if manual is specified nothing will be done for this component so
// you'll have to add all the gluing code yourself.
//
// Options are passed like so:
// > COMPONENT(pool=y,accessors=n)
// or
// > COMPONENT(manual)
#define COMPONENT

/// GEN_COMPONENT_POOLS
//
// Macro used for substitution for the generated component pools
#define GEN_COMPONENT_POOLS

struct component {};

#endif
