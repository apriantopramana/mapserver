/**********************************************************************
 * $Id$
 *
 * mapows.c - OGC Web Services (WMS, WFS) support functions
 *
 **********************************************************************
 * $Log$
 * Revision 1.2  2002/10/04 21:29:41  dan
 * WFS: Added GetCapabilities and basic GetFeature (still some work to do)
 *
 * Revision 1.1  2002/09/03 03:19:51  dan
 * Set the bases for WFS Server support + moved some WMS/WFS stuff to mapows.c
 *
 **********************************************************************/

#include "map.h"

#if defined(USE_WMS_SVR) || defined (USE_WFS_SVR)

/*
** msOWSDispatch() is the entry point for any OWS request (WMS, WFS, ...)
** - If this is a valid request then it is processed and MS_SUCCESS is returned
**   on success, or MS_FAILURE on failure.
** - If this does not appear to be a valid OWS request then MS_DONE
**   is returned and MapServer is expected to process this as a regular
**   MapServer request.
*/
int msOWSDispatch(mapObj *map, char **names, char **values, int numentries)
{
    int status = MS_DONE;

#ifdef USE_WMS_SVR
    if ((status = msWMSDispatch(map, names, values, numentries)) != MS_DONE )
        return status;
#endif
#ifdef USE_WFS_SVR
    if ((status = msWFSDispatch(map, names, values, numentries)) != MS_DONE )
        return status;
#endif

    return MS_DONE;  /* Not a WMS/WFS request... let MapServer handle it */
}


/*
** msRenameLayer()
*/
static int msRenameLayer(layerObj *lp, int count)
{
    char *newname;
    newname = (char*)malloc((strlen(lp->name)+5)*sizeof(char));
    if (!newname) 
    {
        msSetError(MS_MEMERR, NULL, "msRenameLayer()");
        return MS_FAILURE;
    }
    sprintf(newname, "%s_%2.2d", lp->name, count);
    free(lp->name);
    lp->name = newname;
    
    return MS_SUCCESS;
}

/*
** msOWSMakeAllLayersUnique()
*/
int msOWSMakeAllLayersUnique(mapObj *map)
{
  int i, j;

  // Make sure all layers in the map file have valid and unique names
  for(i=0; i<map->numlayers; i++)
  {
      int count=1;
      for(j=i+1; j<map->numlayers; j++)
      {
          if (map->layers[i].name == NULL || map->layers[j].name == NULL)
          {
              msSetError(MS_MISCERR, 
                         "At least one layer is missing a name in map file.", 
                         "msOWSMakeAllLayersUnique()");
              return MS_FAILURE;
          }
          if (strcasecmp(map->layers[i].name, map->layers[j].name) == 0 &&
              msRenameLayer(&(map->layers[j]), ++count) != MS_SUCCESS)
          {
              return MS_FAILURE;
          }
      }

      // Don't forget to rename the first layer if duplicates were found
      if (count > 1 && msRenameLayer(&(map->layers[i]), 1) != MS_SUCCESS)
      {
          return MS_FAILURE;
      }
  }
  return MS_SUCCESS;
}


/*
** msOWSGetOnlineResource()
**
** Return the online resource for this service.  First try to lookup 
** specified metadata, and if not found then try to build the URL ourselves.
**
** Returns a newly allocated string that should be freed by the caller or
** NULL in case of error.
*/
char * msOWSGetOnlineResource(mapObj *map, const char *metadata_name)
{
    const char *value;
    char *online_resource = NULL;

    // We need this script's URL, including hostname.
    // Default to use the value of the "onlineresource" metadata, and if not
    // set then build it: "http://$(SERVER_NAME):$(SERVER_PORT)$(SCRIPT_NAME)?"
    if ((value = msLookupHashTable(map->web.metadata, (char*)metadata_name))) 
    {
        online_resource = (char*) malloc(strlen(value)+2);

        // Append trailing '?' or '&' if missing.
        strcpy(online_resource, value);
        if (strchr(online_resource, '?') == NULL)
            strcat(online_resource, "?");
        else
        {
            char *c;
            c = online_resource+strlen(online_resource)-1;
            if (*c != '?' && *c != '&')
                strcpy(c+1, "&");
        }
    }
    else 
    {
        const char *hostname, *port, *script, *protocol="http";
        hostname = getenv("SERVER_NAME");
        port = getenv("SERVER_PORT");
        script = getenv("SCRIPT_NAME");

        // HTTPS is set by Apache to "on" in an HTTPS server ... if not set
        // then check SERVER_PORT: 443 is the default https port.
        if ( ((value=getenv("HTTPS")) && strcasecmp(value, "on") == 0) ||
             ((value=getenv("SERVER_PORT")) && atoi(value) == 443) )
        {
            protocol = "https";
        }

        if (hostname && port && script) {
            online_resource = (char*)malloc(sizeof(char)*(strlen(hostname)+strlen(port)+strlen(script)+10));
            if (online_resource) 
                sprintf(online_resource, "%s://%s:%s%s?", protocol, hostname, port, script);
        }
        else 
        {
            msSetError(MS_CGIERR, "Impossible to establish server URL.  Please set \"%s\" metadata.", "msWMSCapabilities()", metadata_name);
            return NULL;
        }
    }

    if (online_resource == NULL) 
    {
        msSetError(MS_MEMERR, NULL, "msWMSCapabilities");
        return NULL;
    }

    return online_resource;
}



/*
** msOWSPrintMetadata()
**
** Attempt to output a capability item.  If corresponding metadata is not 
** found then one of a number of predefined actions will be taken. 
** If a default value is provided and metadata is absent then the 
** default will be used.
*/

int msOWSPrintMetadata(hashTableObj metadata, const char *name, 
                       int action_if_not_found, const char *format, 
                       const char *default_value) 
{
    const char *value;
    int status = MS_NOERR;

    if((value = msLookupHashTable(metadata, (char*)name)))
    { 
        printf(format, value);
    }
    else
    {
        if (action_if_not_found == OWS_WARN)
        {
            printf("<!-- WARNING: Mandatory metadata '%s' was missing in this context. -->\n", name);
            status = action_if_not_found;
        }

        if (default_value)
            printf(format, default_value);
    }

    return status;
}

/*
** msOWSPrintGroupMetadata()
**
** Attempt to output a capability item.  If corresponding metadata is not 
** found then one of a number of predefined actions will be taken. 
** If a default value is provided and metadata is absent then the 
** default will be used.
*/

int msOWSPrintGroupMetadata(mapObj *map, char* pszGroupName, const char *name, 
                            int action_if_not_found, const char *format, 
                            const char *default_value) 
{
    const char *value;
    int status = MS_NOERR;
    int i;

    for (i=0; i<map->numlayers; i++)
    {
       if (map->layers[i].group && (strcmp(map->layers[i].group, pszGroupName) == 0) && map->layers[i].metadata)
       {
         if((value = msLookupHashTable(map->layers[i].metadata, (char*)name)))
         { 
            printf(format, value);
            return status;
         }
       }
    }

    if (action_if_not_found == OWS_WARN)
    {
       printf("<!-- WARNING: Mandatory metadata '%s' was missing in this context. -->\n", name);
       status = action_if_not_found;
    }

    if (default_value)
      printf(format, default_value);
   
    return status;
}

/* msOWSPrintParam()
**
** Same as printMetadata() but applied to mapfile parameters.
**/
int msOWSPrintParam(const char *name, const char *value, 
                    int action_if_not_found, const char *format, 
                    const char *default_value) 
{
    int status = MS_NOERR;

    if(value && strlen(value) > 0)
    { 
        printf(format, value);
    }
    else
    {
        if (action_if_not_found == OWS_WARN)
        {
            printf("<!-- WARNING: Mandatory mapfile parameter '%s' was missing in this context. -->\n", name);
            status = action_if_not_found;
        }

        if (default_value)
            printf(format, default_value);
    }

    return status;
}

/* msOWSPrintMetadataList()
**
** Prints comma-separated lists metadata.  (e.g. keywordList)
**/
int msOWSPrintMetadataList(hashTableObj metadata, const char *name, 
                           const char *startTag, const char *endTag,
                           const char *itemFormat) 
{
    const char *value;
    if((value = msLookupHashTable(metadata, (char*)name))) 
    {
      char **keywords;
      int numkeywords;
      
      keywords = split(value, ',', &numkeywords);
      if(keywords && numkeywords > 0) {
        int kw;
	printf("%s", startTag);
	for(kw=0; kw<numkeywords; kw++) 
            printf(itemFormat, keywords[kw]);
	printf("%s", endTag);
	msFreeCharArray(keywords, numkeywords);
      }
      return MS_TRUE;
    }
    return MS_FALSE;
}

/*
**
*/
void msOWSPrintLatLonBoundingBox(const char *tabspace, 
                                 rectObj *extent, projectionObj *srcproj)
{
  rectObj ll_ext;
  ll_ext = *extent;

  if (srcproj->numargs > 0 && !pj_is_latlong(srcproj->proj)) {
    msProjectRect(srcproj, NULL, &ll_ext);
  }

  printf("%s<LatLonBoundingBox minx=\"%g\" miny=\"%g\" maxx=\"%g\" maxy=\"%g\" />\n", 
         tabspace, ll_ext.minx, ll_ext.miny, ll_ext.maxx, ll_ext.maxy);
}

/*
** Emit a bounding box if we can find projection information.
*/
void msOWSPrintBoundingBox(const char *tabspace, 
                           rectObj *extent, 
                           projectionObj *srcproj,
                           hashTableObj metadata ) 
{
    const char	*value, *resx, *resy;

    value = msGetEPSGProj(srcproj, metadata, MS_TRUE);
    
    if( value != NULL )
    {
        printf("%s<BoundingBox SRS=\"%s\"\n"
               "%s            minx=\"%g\" miny=\"%g\" maxx=\"%g\" maxy=\"%g\"",
               tabspace, value, 
               tabspace, extent->minx, extent->miny, 
               extent->maxx, extent->maxy);

        if( ((resx = msLookupHashTable( metadata, "wms_resx" )) != NULL ||
             (resx = msLookupHashTable( metadata, "wfs_resx" )) != NULL )
            && ((resy = msLookupHashTable( metadata, "wms_resy" )) != NULL ||
                (resy = msLookupHashTable( metadata, "wfs_resy" )) != NULL ) )
            printf( "\n%s            resx=\"%s\" resy=\"%s\"",
                    tabspace, resx, resy );
 
        printf( " />\n" );
    }
}


/* msEncodeHTMLEntities()
**
** Return a copy of string after replacing some problematic chars with their
** HTML entity equivalents.
**
** The replacements performed are:
**  '&' -> "&amp;", '"' -> "&quot;", '<' -> "&lt;" and '>' -> "&gt;"
**/
char *msEncodeHTMLEntities(const char *string) 
{
    int buflen, i;
    char *newstring;
    const char *c;

    // Start with 100 extra chars for replacements... 
    // should be good enough for most cases
    buflen = strlen(string) + 100;
    newstring = (char*)malloc(buflen+1*sizeof(char*));
    if (newstring == NULL)
    {
        msSetError(MS_MEMERR, NULL, "msEncodeHTMLEntities()");
        return NULL;
    }

    for(i=0, c=string; *c != '\0'; c++)
    {
        // Need to realloc buffer?
        if (i+6 > buflen)
        {
            // If we had to realloc then this string must contain several
            // entities... so let's go with twice the previous buffer size
            buflen *= 2;
            newstring = (char*)realloc(newstring, buflen+1*sizeof(char*));
            if (newstring == NULL)
            {
                msSetError(MS_MEMERR, NULL, "msEncodeHTMLEntities()");
                return NULL;
            }
        }

        switch(*c)
        {
          case '&':
            strcpy(newstring+i, "&amp;");
            i += 5;
            break;
          case '"':
            strcpy(newstring+i, "&quot;");
            i += 6;
            break;
          case '<':
            strcpy(newstring+i, "&lt;");
            i += 4;
            break;
          case '>':
            strcpy(newstring+i, "&gt;");
            i += 4;
            break;
          default:
            newstring[i++] = *c;
        }
    }

    return newstring;
}



/*
** msOWSGetLayerExtent()
**
** Try to establish layer extent, first looking for "extent" metadata, and
** if not found then open layer to read extent.
**
** __TODO__ Replace metadata with EXTENT param in layerObj???
*/
int msOWSGetLayerExtent(mapObj *map, layerObj *lp, rectObj *ext)
{
  static char *value;

  if ((value = msLookupHashTable(lp->metadata, "wms_extent")) != NULL ||
      (value = msLookupHashTable(lp->metadata, "wfs_extent")) != NULL )
  {
    char **tokens;
    int n;

    tokens = split(value, ' ', &n);
    if (tokens==NULL || n != 4) {
      msSetError(MS_WMSERR, "Wrong number of arguments for EXTENT metadata.",
                 "msWMSGetLayerExtent()");
      return MS_FAILURE;
    }
    ext->minx = atof(tokens[0]);
    ext->miny = atof(tokens[1]);
    ext->maxx = atof(tokens[2]);
    ext->maxy = atof(tokens[3]);

    msFreeCharArray(tokens, n);
    return MS_SUCCESS;
  }
  else if (lp->type == MS_LAYER_RASTER)
  {
    // __TODO__ We need getExtent() for rasters... use metadata for now.
    return MS_FAILURE; 
  }
  else
  {
    if (msLayerOpen(lp, map->shapepath) == MS_SUCCESS) {
      int status;
      status = msLayerGetExtent(lp, ext);
      msLayerClose(lp);
      return status;
    }
  }

  return MS_FAILURE;
}



#endif /* USE_WMS_SVR || USE_WFS_SVR */



