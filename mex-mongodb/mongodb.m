function retval = mongodb(filter_field, filter_condition, filter_value, output_field, limit)
            hostname = '192.168.1.136'
            port = 27017
            username = 'root'
            password = 'access'
            database = 'cpu_stat'
            output = 'cell'
            collection = 'cpu_util'
            retval = mongodb_(hostname, port, username, password, database, collection, limit, filter_field, filter_condition,filter_value, output_field);
            
            if numel(retval) > 0
                switch output
                    case 'cell'
                        retval = cellfun(@(x) str2double(x), retval);
                        return
                    otherwise
                        return
                endswitch
            endif
endfunction
