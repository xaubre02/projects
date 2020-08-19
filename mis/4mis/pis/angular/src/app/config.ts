export const apiHost = 'http://localhost:8080/pis.bakery/rest';

export const userRoles = {
  manager:          'manažer',
  baker:            'pekař',
  materialstoreman: 'skladník',
  pastrystoreman:   'výdejce',
  customer:         'zákazník'
};

export const userRolesDb = {
  manager:          'manager',
  baker:            'baker',
  materialstoreman: 'materialstoreman',
  pastrystoreman:   'pastrystoreman',
  customer:         'customer'
};

export const serviceConfiguration = {
  users: {
    apiLogin       : apiHost + '/users/login'
  },
  employees: {
    apiList        : apiHost + '/employees/list',
    apiPostEmployee: apiHost + '/employees',
    apiGetEmployee : apiHost + '/employees/',
    apiPutEmployee : apiHost + '/employees/'
  },
  customers: {
    apiList        : apiHost + '/customers/list',
    apiPostCustomer: apiHost + '/customers',
    apiGetCustomer : apiHost + '/customers/',
    apiPutCustomer : apiHost + '/customers/'
  },
  pastry: {
    apiList        : apiHost + '/pastry/list',
    apiPostPastry  : apiHost + '/pastry',
    apiDeletePastry: apiHost + '/pastry/',
    apiPutPastry   : apiHost + '/pastry/',
    apiPostCategory: apiHost + '/pastrycategories',
    apiDeleteCategory : apiHost + '/pastrycategories/',
    apiPutCategory : apiHost + '/pastrycategories/',
    apiAmount      : apiHost + '/pastrystorage/list',
    apiEnter       : apiHost + '/pastrystorage/enter',
    apiEnterList   : apiHost + '/pastrystorage/enter/list',
    apiWithdraw    : apiHost + '/pastrystorage/withdraw',
    apiWithdrawList: apiHost + '/pastrystorage/withdraw/list',
    apiCategoryList: apiHost + '/pastrycategories/list'
  },
  materials: {
    apiList        : apiHost + '/materials/list',
    apiPost        : apiHost + '/materials',
    apiPut         : apiHost + '/materials/',
    apiDelete      : apiHost + '/materials/',
    apiAmount      : apiHost + '/materialstorage/list',
    apiEnter       : apiHost + '/materialstorage/enter',
    apiEnterList   : apiHost + '/materialstorage/enter/list',
    apiWithdraw    : apiHost + '/materialstorage/withdraw',
    apiWithdrawList: apiHost + '/materialstorage/withdraw/list'
  },
  pastryOrders: {
    apiList        : apiHost + '/orders/list',
    apiState       : apiHost + '/orders/state/%STATE%',
    apiDelivery    : apiHost + '/orders/deliveryDate/%DATE%',
    apiCustomer    : apiHost + '/orders/customer/%USER%',
    apiOrderGet    : apiHost + '/orders/%ORDER%',
    apiOrderPut    : apiHost + '/orders/%ORDER%',
    apiOrderPost   : apiHost + '/orders',
    apiOrderCancel : apiHost + '/orders/cancel/%ORDER%'
  },
  materialOrders: {
    apiList        : apiHost + '/materialorders/list',
    apiState       : apiHost + '/materialorders/delivered/%STATE%',
    apiOrderGet    : apiHost + '/materialorders/%ORDER%',
    apiOrderPost   : apiHost + '/materialorders',
    apiOrderAccept : apiHost + '/materialorders/accept/%ORDER%',
    apiOrderCancel : apiHost + '/materialorders/cancel/%ORDER%'
  },
  productionPlans: {
    apiList        : apiHost + '/productionPlans/list',
    apiPlanGet     : apiHost + '/productionPlans/%PLAN%',
    apiPlanGetDate : apiHost + '/productionPlans/productionDate/%DATE%',
    apiPlanCreate  : apiHost + '/productionPlans/create/%DATE%',
    apiPlanPost    : apiHost + '/productionPlans',
    apiPlanPut     : apiHost + '/productionPlans/%PLAN%',
  }
};
