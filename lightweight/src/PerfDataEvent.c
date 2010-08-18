/****************************************************************************
 *  Copyright 2003-2010 Dorian C. Arnold, Philip C. Roth, Barton P. Miller  *
 *                  Detailed MRNet usage rights in "LICENSE" file.          *
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#if !defined(os_windows)
#include <unistd.h>
#include <inttypes.h>
#endif //!defined(os_windows)

#include "PerfDataEvent.h" 
#include "PerfDataSysEvent.h"
#include "map.h"
#include "mrnet_lightweight/Types.h"
#include "utils_lightweight.h"
#include "vector.h"

#define PERFDATA_MET_FLAG(x) ((char)(1 << x))

PerfDataMgr_t* new_PerfDataMgr_t()
{
    int i;
    mrn_map_t* newmap;
    PerfDataMgr_t* newperf = (PerfDataMgr_t*)malloc(sizeof(PerfDataMgr_t));
    assert(newperf != NULL);
    newperf->the_data = new_empty_vector_t();
    memset(newperf->active_metrics, 0, sizeof(newperf->active_metrics));
    for (i = 0; i < PERFDATA_MAX_CTX; i++) {
        newmap = new_map_t();
        pushBackElement(newperf->the_data, newmap);
    }
    return newperf;
}

char * PerfDataMgr_get_MetricName(PerfDataMgr_t* perf_data,
                                  perfdata_metric_t met)
{
    return perfdata_metric_info[(unsigned)met].name;
}

char * PerfDataMgr_get_MetricUnits(PerfDataMgr_t* perf_data,
                                   perfdata_metric_t met)
{
    return perfdata_metric_info[(unsigned)met].units;
}

char * PerfDataMgr_get_MetricDescription(PerfDataMgr_t* perf_data,
                                         perfdata_metric_t met)
{
    return perfdata_metric_info[(unsigned)met].description;
}

perfdata_mettype_t PerfDataMgr_get_MetricType(PerfDataMgr_t* perf_data,
                                              perfdata_metric_t met)
{
    return perfdata_metric_info[(unsigned)met].type;
}

void PerfDataMgr_enable(PerfDataMgr_t* perf_data,
                        perfdata_metric_t met,
                        perfdata_context_t ctx)
{
    int ndx = ctx;
    if (ndx < PERFDATA_MAX_CTX) {
        if (met < PERFDATA_MAX_MET) {
            perf_data->active_metrics[ndx] |= PERFDATA_MET_FLAG(met);
        }
    }
}

void PerfDataMgr_disable(PerfDataMgr_t* perf_data,
                         perfdata_metric_t met, 
                         perfdata_context_t ctx)
{
    int ndx = ctx;
    if (ndx < PERFDATA_MAX_CTX) {
        if (met < PERFDATA_MAX_MET) {
            perf_data->active_metrics[ndx] &= ~(PERFDATA_MET_FLAG(met));
        }
    }
}

int PerfDataMgr_is_Enabled(PerfDataMgr_t* perf_data, 
                           perfdata_metric_t met, 
			   perfdata_context_t ctx)
{
    int ndx = ctx;
    if (ndx < PERFDATA_MAX_CTX) {
        if (perf_data->active_metrics[ndx] & PERFDATA_MET_FLAG(met)) {
            return true;
        } 
    }
    return false;
}

void PerfDataMgr_add_DataInstance(PerfDataMgr_t* perf_data,
                                  perfdata_metric_t met,
                                  perfdata_context_t ctx,
                                  perfdata_t data)
{
    perfdata_t* newdata;
    mrn_map_t* ctx_map;
    vector_t* met_data;

    mrn_dbg_func_begin();
    newdata = (perfdata_t*)malloc(sizeof(perfdata_t));
    assert(newdata != NULL);
    *newdata = data;

    ctx_map = (mrn_map_t*)(perf_data->the_data->vec[(unsigned)ctx]);
    if (ctx_map->size > 0) {
        met_data = (vector_t*)(get_val(ctx_map,met));
        if ((met_data!=NULL) && (met_data->size)) {
            pushBackElement(met_data, newdata);
        } else {
            met_data = new_empty_vector_t();
            pushBackElement(met_data, newdata);
        }
    }
    else {
        met_data = new_empty_vector_t();
        pushBackElement(met_data, newdata);
        insert(ctx_map, met, met_data);
    }

    mrn_dbg_func_end();
}

perfdata_t PerfDataMgr_get_DataValue(PerfDataMgr_t* perf_data, 
                                     perfdata_metric_t met,
                                     perfdata_context_t ctx)
{
    mrn_map_t* ctx_map;
    vector_t* met_data;
    perfdata_t zero;

    mrn_dbg_func_begin();

    ctx_map = (mrn_map_t*)(perf_data->the_data->vec[(unsigned)ctx]);
    if(ctx_map->size > 0) {
        met_data = (vector_t*)(get_val(ctx_map, met));
        if( (met_data!=NULL) && (met_data->size)) {
            mrn_dbg_func_end();
            return *(perfdata_t*)met_data->vec[0];
        }
    }
    // if ctx_map or met_data are empty...

    memset(&zero, 0, sizeof(zero));
    mrn_dbg_func_end();
    return zero;
}

void PerfDataMgr_set_DataValue(PerfDataMgr_t* perf_data,
                               perfdata_metric_t met,
                               perfdata_context_t ctx,
                               perfdata_t data)
{
    perfdata_t* newdata;
    mrn_map_t* ctx_map;
    vector_t* met_data;
    
    mrn_dbg_func_begin();
    newdata = (perfdata_t*)malloc(sizeof(perfdata_t));
    assert(newdata != NULL);
    *newdata = data;

    ctx_map = (mrn_map_t*)(perf_data->the_data->vec[(unsigned)ctx]);
    mrn_dbg(5, mrn_printf(FLF, stderr, "ctx_map->size=%d\n", ctx_map->size));
    if (ctx_map->root) {
        mrn_dbg(5, mrn_printf(FLF, stderr, "ctx_map->root is not NULL\n"));
    }
    else { 
        mrn_dbg(5, mrn_printf(FLF, stderr, "ctx_map->root _is_ NULL\n"));
    }
    if (ctx_map->size > 0) {
        met_data = (vector_t*)(get_val(ctx_map, met));
        if ((met_data!=NULL) && (met_data->size)) {
            clear(met_data);
        }
        if (met_data==NULL) {
            met_data = new_empty_vector_t();
        }
        pushBackElement(met_data, newdata);
    }
    else {
        met_data = new_empty_vector_t();
        pushBackElement(met_data, newdata);
        insert(ctx_map, met, met_data);
    }
    mrn_dbg_func_end();
}

void PerfDataMgr_get_MemData(PerfDataMgr_t* perf_data,
                             perfdata_metric_t metric)
{
    double vsize=0, psize=0;
    perfdata_t val;
	
    mrn_dbg_func_begin();

    PerfDataSysMgr_get_MemUsage(&vsize, &psize);
    mrn_dbg(5, mrn_printf(FLF, stderr, "print_PerfData vsize %f psize %f\n", vsize, psize));

    if (metric == PERFDATA_MET_MEM_VIRT_KB) {
        val.d = vsize;
        PerfDataMgr_add_DataInstance(perf_data, 
                                    PERFDATA_MET_MEM_VIRT_KB,
                                    PERFDATA_CTX_NONE,
                                    val);
    } else if (metric == PERFDATA_MET_MEM_PHYS_KB) {
        val.d = psize;
        PerfDataMgr_add_DataInstance(perf_data,
                                    PERFDATA_MET_MEM_PHYS_KB,
                                    PERFDATA_CTX_NONE,
                                    val);   
    }
    mrn_dbg(5, mrn_printf(FLF, stderr, "Are we really segfaulting here?\n"));
    mrn_dbg_func_end();
}

void PerfDataMgr_print(PerfDataMgr_t* perf_data,
                       perfdata_metric_t met,
                       perfdata_context_t ctx) 
{
    vector_t* data = new_empty_vector_t();
    struct timeval tv;
    perfdata_metinfo_t* mi;
    int size;
    char* report;
    int k;

    mrn_dbg_func_begin();

    PerfDataMgr_collect(perf_data, met, ctx, data); 

    gettimeofday(&tv, NULL);

    mi = perfdata_metric_info + (unsigned)met;

#ifndef os_linux
    size = 36; // "PERFDATA @ T=" + "%d" + "%06dsec: "
    size += strlen(mi->name);
    size += strlen(perfdata_context_names[(unsigned)ctx]);
	
    report= (char*)malloc(sizeof(char)*size);
    assert(report);
    sprintf( report, "PERFDATA @ T=%d.%06dsec: %s %s -",
             (int)tv.tv_sec-MRN_RELEASE_DATE_SECS, (int)tv.tv_usec, mi->name, 
              perfdata_context_names[(unsigned)ctx] );
#else
    if (!asprintf(&report, "PERFDATA @ T=%d.%.06dsec: %s %s -",
            (int)tv.tv_sec-MRN_RELEASE_DATE_SECS, (int)tv.tv_usec, mi->name, perfdata_context_names[(unsigned)ctx])) {
        mrn_dbg(5, mrn_printf(FLF, stderr, "asprintf to report data failed\n"));
    }
#endif

    for (k = 0; k < data->size; k++) {
        if (mi->type == PERFDATA_TYPE_UINT) {
            mrn_printf(FLF, stderr, "%s %u %s\n",
                    report, ((perfdata_t*)data->vec[k])->u, mi->units);
        } else if (mi->type == PERFDATA_TYPE_INT) {
            mrn_printf(FLF, stderr, "%s %i %s\n",
                    report, ((perfdata_t*)data->vec[k])->i, mi->units);
        } else if (mi->type == PERFDATA_TYPE_FLOAT) {
            mrn_printf(FLF, stderr, "%s %lf %s\n",
                    report, ((perfdata_t*)data->vec[k])->d, mi->units);
        }
    }
    if (report != NULL)
        free(report);

    mrn_dbg_func_end();
}

void PerfDataMgr_collect(PerfDataMgr_t* perf_data,
                         perfdata_metric_t met,
                         perfdata_context_t ctx,
                         vector_t* data)
{
    mrn_map_t* ctx_map;
    vector_t* met_data;

    mrn_dbg_func_begin();

    ctx_map = (mrn_map_t*)(perf_data->the_data->vec[(unsigned)ctx]);
    if (ctx_map->size > 0) {
        //vector_t* met_data = ctx_map[met];
        met_data = (vector_t*)(get_val(ctx_map, met));
        //data = met_data;
        if ((met_data!= NULL) && (met_data->size)) {
            copy_vector(met_data, data);
            clear(met_data);    
        }
    } else {
        met_data = new_empty_vector_t();
        insert(ctx_map, met, met_data);
    }

    mrn_dbg_func_end();
}

