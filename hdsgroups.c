/* Wrapper source file that contains both the routine for registering
 * a locator with a group (hdsLink) and the corresponding routine for
 * freeing locators in a group. The two are combined to allow the routines
 * to easily share a single data structure for group membership.
 */

#include "ems.h"
#include "sae_par.h"
#include "star/one.h"
#include "hds1.h"
#include "dat1.h"
#include "hds.h"

#include "dat_err.h"

/* Have a simple hash table keyed by group name with
 * values being an array of locators. HDS groups are not
 * used very often (mainly in ADAM) so for now there is no
 * need for high performance data structures. We do store
 * the group name in the locator struct to allow fast
 * retrieval of the group name without having to scan
 * all the hash tables.
 */

/* Use the uthash macros: https://github.com/troydhanson/uthash */
#include "uthash.h"
#include "utarray.h"

/* We do not want to clone so we just copy the pointer */
/* UTarray takes a copy so we create a mini struct to store
   the actual pointer. */
typedef struct {
  HDSLoc * locator;    /* Actual HDS locator */
} HDSelement;
UT_icd locators_icd = { sizeof(HDSelement *), NULL, NULL, NULL };

typedef struct {
  char grpname[DAT__SZGRP+1]; /* Group name: the key */
  UT_array * locators;        /* Pointer to hash of element structs */
  UT_hash_handle hh;          /* Mandatory for UTHASH */
} HDSgroup;

/* Declare the hash */
HDSgroup *groups = NULL;

/*
*+
*  Name:
*     hdsLink

*  Purpose:
*     Link locator group

*  Language:
*     Starlink ANSI C

*  Type of Module:
*     Library routine

*  Invocation:
*     hdsLink(HDSLoc *locator, const char *group_str, int *status);

*  Arguments:
*     locator = HDSLoc * (Given and Returned)
*        Object locator
*     group = const char * (Given)
*        Group name.
*     status = int* (Given and Returned)
*        Pointer to global status.

*  Description:
*     Link a locator to a group.

*  Authors:
*     TIMJ: Tim Jenness (Cornell)
*     {enter_new_authors_here}

*  Notes:
*     - See also hdsFlush and hdsGroup.
*     - Once a locator is registered with a group it should not be annuled
*       by the caller. It can only be annuled by calling hdsFlush.
*     - A locator can only be assigned to a single group.

*  History:
*     2014-10-17 (TIMJ):
*        Initial version
*     {enter_further_changes_here}

*  Copyright:
*     Copyright (C) 2014 Cornell University
*     All Rights Reserved.

*  Licence:
*     Redistribution and use in source and binary forms, with or
*     without modification, are permitted provided that the following
*     conditions are met:
*
*     - Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*
*     - Redistributions in binary form must reproduce the above
*       copyright notice, this list of conditions and the following
*       disclaimer in the documentation and/or other materials
*       provided with the distribution.
*
*     - Neither the name of the {organization} nor the names of its
*       contributors may be used to endorse or promote products
*       derived from this software without specific prior written
*       permission.
*
*     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
*     CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*     INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
*     MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
*     CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*     SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*     LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*     USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
*     AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*     LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
*     IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
*     THE POSSIBILITY OF SUCH DAMAGE.

*  Bugs:
*     {note_any_bugs_here}
*-
*/


int
hdsLink(HDSLoc *locator, const char *group_str, int *status) {
  HDSgroup *entry = NULL;
  HDSelement elt;
  memset(&elt, 0, sizeof(elt));

  if (*status != SAI__OK) return *status;

  /* Check that a group name is not already set */
  if ( (locator->grpname)[0] != '\0') {
    *status = DAT__GRPIN;
    emsRepf("hdsLink", "This locator has already been assigned to group '%s'",
            status, locator->grpname );
    return *status;
  }

  /* Now copy the group name to the locator */
  one_strlcpy( locator->grpname, group_str, sizeof(locator->grpname), status );

  /* See if this entry already exists in the hash */
  HASH_FIND_STR( groups, group_str, entry );
  if (!entry) {
    entry = calloc( 1, sizeof(HDSgroup) );
    one_strlcpy( entry->grpname, group_str, sizeof(entry->grpname), status );
    utarray_new( entry->locators, &locators_icd);
    HASH_ADD_STR( groups, grpname, entry );
  }

  /* Now we have the entry, we need to store the locator inside.
     We do not clone the locator, the locator is now owned by the group. */
  printf("Storing %p in group %s\n", locator, group_str);
  elt.locator = locator;
  utarray_push_back( entry->locators, &elt );

  return *status;
}
/*
*+
*  Name:
*     hdsFlush

*  Purpose:
*     Flush locator group

*  Language:
*     Starlink ANSI C

*  Type of Module:
*     Library routine

*  Invocation:
*     hdsFlush( const char *group_str, int *status);

*  Arguments:
*     group = const char * (Given)
*        Group name
*     status = int* (Given and Returned)
*        Pointer to global status.

*  Description:
*     Annuls all locators currently assigned to a specified locator group.

*  Authors:
*     TIMJ: Tim Jenness (Cornell)
*     {enter_new_authors_here}

*  Notes:
*     - See also hdsLink and hdsGroup

*  History:
*     2014-10-17 (TIMJ):
*        Initial version
*     {enter_further_changes_here}

*  Copyright:
*     Copyright (C) 2014 Cornell University
*     All Rights Reserved.

*  Licence:
*     Redistribution and use in source and binary forms, with or
*     without modification, are permitted provided that the following
*     conditions are met:
*
*     - Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*
*     - Redistributions in binary form must reproduce the above
*       copyright notice, this list of conditions and the following
*       disclaimer in the documentation and/or other materials
*       provided with the distribution.
*
*     - Neither the name of the {organization} nor the names of its
*       contributors may be used to endorse or promote products
*       derived from this software without specific prior written
*       permission.
*
*     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
*     CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*     INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
*     MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
*     CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*     SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*     LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*     USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
*     AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*     LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
*     IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
*     THE POSSIBILITY OF SUCH DAMAGE.

*  Bugs:
*     {note_any_bugs_here}
*-
*/

int
hdsFlush( const char *group_str, int *status) {
  HDSgroup * entry = NULL;
  HDSelement * elt = NULL;

  if (*status != SAI__OK) return *status;
  printf("Flushing locator group %s\n", group_str);
  /* See if this entry already exists in the hash */
  HASH_FIND_STR( groups, group_str, entry );
  if (!entry) {
    *status = DAT__GRPIN;
    emsRepf("hdsFlush_1", "Can not flush a group named '%s' that does not exist",
            status, group_str );
    return *status;
  }

  /* Read all the elements from the entry and annul them */
  for ( elt = (HDSelement *)utarray_front(entry->locators);
        elt != NULL;
        elt = (HDSelement *)utarray_next(entry->locators, elt )) {
    char name_str[DAT__SZNAM+1];
    HDSLoc * loc = elt->locator;
    printf("Annulling element: %p\n", loc );

    datAnnul( &loc, status );
  }

  /* Free the array and delete the hash entry */
  utarray_free( entry->locators );
  HASH_DEL( groups, entry );

  return *status;
}

/* Remove a locator from a group. This is currently a private
   routine to allow datAnnul to free a locator that has been
   associated with a group outside of hdsFlush. This is quite
   probably a bug but a bug that is currently prevalent in
   SUBPAR which seems to store locators in groups and then
   frees them anyhow. This removal will prevent hdsFlush
   attempting to also free the locator.

   Returns true if the locator was removed.

*/

hdsbool_t
hds1RemoveLocator( const HDSLoc * loc, int *status ) {
  HDSgroup * entry = NULL;
  HDSelement * elt = NULL;
  const char * grpname;
  int pos = -1;
  unsigned int len = 0;
  unsigned int i = 0;
  int removed = 0;

  if (*status != SAI__OK) return removed;

  /* Not associated with a group */
  grpname = loc->grpname;
  if (grpname[0] == '\0') return removed;

  /* Look for the entry associated with this name */
  HASH_FIND_STR( groups, grpname, entry );
  if ( !entry ) return removed;

  len = utarray_len( entry->locators );
  /* Read all the elements from the entry, looking for the relevant one */
  for ( i = 0; i < len; i++) {
    HDSLoc * thisloc;
    elt = (HDSelement *)utarray_eltptr( entry->locators, i );
    thisloc = elt->locator;
    if (thisloc == loc) {
      pos = i;
      break;
    }
  }

  if (pos > -1) {
    unsigned int upos = pos;
    utarray_erase( entry->locators, upos, 1 );
    removed = 1;
  }

  return removed;
}
