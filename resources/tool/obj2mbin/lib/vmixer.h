#ifndef _MBIN_VMIXER_H_
#define _MBIN_VMIXER_H_
#include "vattr.h"
#include "vindex.h"


int vmixer_index_compose_from_vertex(IndexAttributeList *,
                                        VertexAttributeList **, VertexBuffer *,
                                        IndexBuffer *);

void vmixer_attribute_line_direction(const VertexAttributeList *,
                                     const IndexAttributeList *,
                                     VertexAttributeList *);

#endif
