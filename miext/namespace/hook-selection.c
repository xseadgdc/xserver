#define HOOK_NAME "selection"

#include <dix-config.h>

#include <stdio.h>

#include "dix/selection_priv.h"

#include "namespace.h"
#include "hooks.h"

void hookSelectionFilter(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XNS_HOOK_HEAD(SelectionFilterParamRec);

    /* no rewrite if client is in root namespace */
    if (subj->ns->superPower)
        return;

    char selname[PATH_MAX] = { 0 };
    snprintf(selname, sizeof(selname)-1, "NS:%s:%s", subj->ns->id, NameForAtom(param->selection));
    Atom realSelection = MakeAtom(selname, strlen(selname), TRUE);

    XNS_HOOK_LOG("selection name: %s id=%d orig=%d\n", selname, realSelection, param->selection);

    switch (param->op) {
        case SELECTION_FILTER_GETOWNER:
            param->selection = realSelection;
            XNS_HOOK_LOG("SELECTION_FILTER_GETOWNER selection=%d\n", param->selection);
        break;
        case SELECTION_FILTER_SETOWNER:
            param->selection = realSelection;
            XNS_HOOK_LOG("SELECTION_FILTER_SETOWNER selection=%d\n", param->selection);
            // TODO: check whether window really belongs to the client
        break;
        case SELECTION_FILTER_CONVERT:
            param->selection = realSelection;
            XNS_HOOK_LOG("SELECTION_FILTER_CONVERT selection=%d\n", param->selection);
        break;
        case SELECTION_FILTER_EV_REQUEST:
            XNS_HOOK_LOG("SELECTION_FILTER_EV_REQUEST selection=%d\n", param->selection);
        break;
        case SELECTION_FILTER_EV_CLEAR:
            XNS_HOOK_LOG("SELECTION_FILTER_EV_CLEAR selection=%d\n", param->selection);
        break;
        default:
            XNS_HOOK_LOG("unknown op: %d selection=%d\n", param->op, param->selection);
        break;
    }
}
