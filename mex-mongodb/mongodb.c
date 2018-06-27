#include <mex.h>
#include <math.h>
#include <mongoc.h>

#define CMOindex(r, c, rows) ( r + c - 1 )
char *hostname, *password, *database, *username, *collection_name;
int port, limit;
mxArray *cell_array_ptr;

mongoc_client_t *client;
mongoc_collection_t *collection;
mongoc_cursor_t *cursor;
const bson_t *doc;
bson_t *query, *opts;
bson_error_t  error;
char *str, *err;
mongoc_uri_t *uri;
char *pstr;
bson_iter_t iter, *child;
const bson_value_t *value;
char *filter_field, *condition, *output_field, *filter_value;

typedef enum {
    HOSTNAME,
    PORT,
    USERNAME,
    PASSWORD,
    DATABASE,
    COLLECTION,
    LIMIT,
    FILTER_FIELD,
    CONDITION,
    FILTER_VALUE,
    OUTPUT_FIELD
} connection_details;

#define DEBUG
void mexFunction (int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[]){
    // --- input checks
    // at least 5 arguments, max 6 arguments
    // DATABASE is the last and optional argument
    if (nrhs >= 4&& nrhs <5 && nlhs != 1) {
        mexErrMsgIdAndTxt( "MATLAB:Mongo:invalidNumInputs",
                        "Four inputs are required.");
    }

    // get hostname
   if ( mxIsChar(prhs[HOSTNAME]) ) {
        hostname = (char *) mxCalloc(mxGetN(prhs[HOSTNAME])+1, sizeof(char));
        mxGetString(prhs[HOSTNAME], hostname, mxGetN(prhs[HOSTNAME])+1);
        #ifdef DEBUG
        	mexPrintf("Hostname: %s\n", hostname);
        #endif
    } else {
        mexErrMsgIdAndTxt("MATLAB::nrhs", "Error setting up mongodb connection: Hostname must be a string.");
    }

    // get port
    if ( mxIsDouble(prhs[PORT]) ) {
        // convert double to integer :: PORT
        double* data = mxGetPr(prhs[PORT]);
        port = (int)floor(data[0]);
        #ifdef DEBUG
        	mexPrintf("Port: %d\n", port);
        #endif
      } else {
        mexErrMsgIdAndTxt("MATLAB::nrhs", "Error setting up mongodb connection: Port must be a double.");
    }

    // get limit 
    if ( mxIsDouble(prhs[LIMIT]) ) {
        // convert double to integer :: LIMIT
        double* data = mxGetPr(prhs[LIMIT]);
        limit = (int)floor(data[0]);
        #ifdef DEBUG
        	mexPrintf("LIMIT: %d\n", limit);
        #endif
      } else {
        mexErrMsgIdAndTxt("MATLAB::nrhs", "Error setting up mongodb connection: Limit must be a double.");
    }

    // get username
        if ( mxIsChar(prhs[USERNAME]) ) {
        username = (char *) mxCalloc(mxGetN(prhs[USERNAME])+1, sizeof(char));
        mxGetString(prhs[USERNAME], username, mxGetN(prhs[USERNAME])+1);
        #ifdef DEBUG
        	mexPrintf("Username: %s\n", username);
        #endif
    } else {
        mexErrMsgIdAndTxt("MATLAB::nrhs", "Error setting up mongodb connection: Username must be a string.");
    }

    // get password
    if ( mxIsChar(prhs[PASSWORD]) ) {
        password = (char *) mxCalloc(mxGetN(prhs[PASSWORD])+1, sizeof(char));
        mxGetString(prhs[PASSWORD], password, mxGetN(prhs[PASSWORD])+1);
        #ifdef DEBUG
        	mexPrintf("Password: %s\n", password);
        #endif
    } else {
        mexErrMsgIdAndTxt("MATLAB::nrhs", "Error setting up mongodb connection: Password must be a string.");
    }

    // get command
    if ( mxIsChar(prhs[COLLECTION]) ) {
        collection_name = (char *) mxCalloc(mxGetN(prhs[COLLECTION])+1, sizeof(char));
        mxGetString(prhs[COLLECTION], collection_name, mxGetN(prhs[COLLECTION])+1);
        #ifdef DEBUG
        	mexPrintf("Collection: %s\n", collection_name);
        #endif
    } else {
        mexErrMsgIdAndTxt("MATLAB::nrhs", "Error setting up mongodb connection: Command must be a string.");
    }

    // get databasename
    if ( mxIsChar(prhs[DATABASE]) ) {
         database = (char *) mxCalloc(mxGetN(prhs[DATABASE])+1, sizeof(char));
            mxGetString(prhs[DATABASE], database, mxGetN(prhs[DATABASE])+1);
            #ifdef DEBUG
                mexPrintf("Database: %s\n", database);
            #endif
     } else {
         database = NULL;
            mexErrMsgIdAndTxt("MATLAB::nrhs", "Error setting up mongodb connection: Database must be a string.");
     }
	if( mxIsChar(prhs[FILTER_FIELD]) ) {
		filter_field = (char *) mxCalloc(mxGetN(prhs[FILTER_FIELD])+1, sizeof(char));
		mxGetString(prhs[FILTER_FIELD], filter_field, mxGetN(prhs[FILTER_FIELD])+1);
            #ifdef DEBUG
		mexPrintf("Filter Field: %s\n", filter_field);
            #endif
	}else {
	    mexErrMsgIdAndTxt("MATLAB:mongo:nrhs", "Error setting up mongo connection: Filter Field must be a string.");
       }

        if( mxIsChar(prhs[CONDITION]) ) {
                condition = (char *) mxCalloc(mxGetN(prhs[CONDITION])+1, sizeof(char));
                mxGetString(prhs[CONDITION], condition, mxGetN(prhs[CONDITION])+1);
            #ifdef DEBUG
                mexPrintf("Condition: %s\n", condition);
            #endif
        }else {
            mexErrMsgIdAndTxt("MATLAB:mongo:nrhs", "Error setting up mongo connection: Filter Field must be a string.");
        }
        if( mxIsChar(prhs[FILTER_VALUE]) ) {
                filter_value = (char *) mxCalloc(mxGetN(prhs[FILTER_VALUE])+1, sizeof(char));
                mxGetString(prhs[FILTER_VALUE], filter_value, mxGetN(prhs[FILTER_VALUE])+1);
            #ifdef DEBUG
                mexPrintf("Filter Value: %s\n", filter_value);
            #endif
        }else {
            mexErrMsgIdAndTxt("MATLAB:mongo:nrhs", "Error setting up mongo connection: Filter Field must be a string.");
        }
        if( mxIsChar(prhs[OUTPUT_FIELD]) ) {
                output_field = (char *) mxCalloc(mxGetN(prhs[OUTPUT_FIELD])+1, sizeof(char));
                mxGetString(prhs[OUTPUT_FIELD], output_field, mxGetN(prhs[OUTPUT_FIELD])+1);
            #ifdef DEBUG
                mexPrintf("Output Field: %s\n", output_field);
            #endif
        }else {
            mexErrMsgIdAndTxt("MATLAB:mongo:nrhs", "Error setting up mongo connection: Filter Field must be a string.");
       }

   mongoc_init ();
   char mongourl[64];
   snprintf(mongourl, 64, "mongodb://%s:%d/?appname=octavia-client", hostname, port);
   uri = mongoc_uri_new_with_error (mongourl, &error);
   if(!uri){
        mexErrMsgIdAndTxt("URI failed %s\n\n",error.message);
   }

   client = mongoc_client_new_from_uri (uri);
   if(!client){
        mexErrMsgIdAndTxt("Client failed %s\n\n",error.message);
   }

   unsigned long long int ts = strtoull(filter_value, &pstr, 10);
   collection = mongoc_client_get_collection (client, database, collection_name);
   query =  BCON_NEW(filter_field, "{", condition, BCON_DATE_TIME(ts), "}");
   opts = BCON_NEW("projection", "{", BCON_UTF8(output_field), BCON_BOOL(true),
                                      "_id", BCON_BOOL(false) , "}",
                   "sort", "{", "_id", BCON_INT32 (-1), "}",
		   "limit", BCON_INT64(limit)
                   );
   cursor = mongoc_collection_find_with_opts(collection, query, opts, NULL);
   if(mongoc_cursor_error(cursor, &error)){
        mexErrMsgIdAndTxt("Error is %s\n",error.message);
   }

   cell_array_ptr = mxCreateCellMatrix(limit, 1);
   int cur_row = 0, cell_index;
   char outputfmt[64];
   while (mongoc_cursor_next (cursor, &doc)) {
      pstr = bson_as_json(doc, NULL);
      bson_free(pstr);

      if (bson_iter_init(&iter, doc)) {
          while (bson_iter_next(&iter)) {
             value = bson_iter_value(&iter);
             if (value->value_type == BSON_TYPE_DOUBLE) {
                 snprintf(outputfmt, 64, "%f", value->value.v_double);
                 cell_index = CMOindex(cur_row, 1, limit);
                 mxSetCell(cell_array_ptr, cell_index, mxCreateString(outputfmt));
             }
             ++cur_row;
             if (cur_row >= limit)
                 break;
          }
      }
   }
   if(mongoc_cursor_error(cursor, &error)){
        mexErrMsgIdAndTxt("Error is %s\n",error.message);
   }

   plhs[0] = cell_array_ptr;
   bson_destroy (query);
   bson_destroy (opts);
   mongoc_cursor_destroy (cursor);
   mongoc_collection_destroy (collection);
   mongoc_client_destroy (client);
   mongoc_cleanup ();

   return;
}
