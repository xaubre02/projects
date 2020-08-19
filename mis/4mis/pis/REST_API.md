# REST API of server
## Error
```
{
    "error": "{ERROR_MESSAGE}"
}
```
## Users
1. rest/users/login (POST) (Permission None)
```
{
    "email": "jannovak@email.com",
    "password": "password"
}
{
    "token": "{JWT_TOKEN}",
    "userId":"1",
    "name":"Jan",
    "surname":"Novák",
    "role":"baker"
}
```
2. rest/users/list (GET) (Permission baker)
```
[
    {
        "userId":"1",
        "born":"1990-12-31",
        "email":"jannova@email.com",
        "name":"Jan",
        "surname":"Novák",
        "role":"baker"
    },
    ...
]
```
## Employees
1. rest/employees/list (GET) (Permission baker)
```
[
    {
        "userId":"1",
        "born":"1990-12-31",
        "email":"jannova@email.com",
        "name":"Jan",
        "surname":"Novák",
        "role":"baker"
    },
    ...
]
```
2. rest/employees/{userId} (GET) (Permission baker)
```
{
    "userId":"1",
    "born":"1990-12-31",
    "email":"jannova@email.com",
    "name":"Jan",
    "surname":"Novák",
    "role":"baker"
}
```
3. rest/employees (POST), (Permission manager)
```
{
    "born":"1990-12-31",
    "email":"jannova@email.com",
    "password": "password",
    "name":"Jan",
    "surname":"Novák",
    "role":"baker"
}
{
    "userId":"1",
    "born":"1990-12-31",
    "email":"jannova@email.com",
    "name":"Jan",
    "surname":"Novák",
    "role":"baker"
}
```
4. rest/employees/{userId} (PUT) same as POST (Permission manager)
## Customers
1. rest/customers/list (GET) (Permission baker)
```
[
    {
        "userId":"1",
        "born":"1990-12-31",
        "email":"jannova@email.com",
        "name":"Jan",
        "surname":"Novák",
        "role":"customer"
    },
    ...
]
```
2. rest/customers/{userId} (GET) (Permission user with {userId})
```
{
    "userId":"1",
    "born":"1990-12-31",
    "email":"jannova@email.com",
    "name":"Jan",
    "surname":"Novák",
    "role":"customer"
}
```
3. rest/customers (POST), (Permission none)
```
{
    "born":"1990-12-31",
    "email":"jannova@email.com",
    "password": "password",
    "name":"Jan",
    "surname":"Novák"
}
{
    "userId": 2,
    "born":"1990-12-31",
    "email":"jannova@email.com",
    "password": "password",
    "name":"Jan",
    "surname":"Novák",
    "role":"customer"
}
```
4. rest/customers/{userId} (PUT) same as POST (Permission user with {userId})
## Material
1. rest/materials/list (GET) (Permission None)
```
[
    {
        "materialId":1,
        "name":"Hladká mouka"
    },
    ...
]
```
2. rest/materials/{materialId} (GET) (Permission None)
```
{
    "materialId":1,
    "name":"Hladká mouka"
}
```
3. rest/materials (POST) (Permission materialstoreman)
```
{
    "name":"Hrubá mouka"
}
{
    "materialId":2,
    "name":"Hrubá mouka"
}
```
4. rest/materials/{materialId} (PUT) Request payload == Response payload (Permission materialstoreman)
```
{
    "materialId":2,
    "name":"Velmi hrubá mouka"
}
```
5. rest/materials/{materialId} (DELETE) (Permission materialstoreman)
## Pastry Category
1. rest/pastrycategories/list (GET) (Permission None)
```
[
    {
        "categoryId":1,
        "name":"Sladké pečivo"
    },
    ...
]
```
2. rest/pastrycategories/{categoryId} (GET) (Permission None)
```
{
    "categoryId":1,
    "name":"Sladké pečivo"
}
```
3. rest/pastrycategories (POST) (Permission manager)
```
{
    "name":"Slanné pečivo"
}
{
    "materialId":2,
    "name":"Slanné pečivo"
}
```
4. rest/pastrycategories/{categoryId} (PUT) Request payload == Response payload (Permission manager)
```
{
    "materialId":2,
    "name":"Velmi slanné pečivo"
}
```
5. rest/pastrycategories/{categoryId} (DELETE) (Permission manager)
## Pastry
1. rest/pastry/list (GET) (Permission None)
```
[
    {
        "pastryId":1,
        "name":"Rohlík",
        "cost":1.5,
        "category":
            {
                "categoryId": 1,
                "name": "Slanné pečivo"
            },
        "materials":
            [
                {
                    "materialId":1,
                    "name":"Hladká mouka",
                    "amount":10.0
                },
                ...
            ]
    },
    ...
]
```
2. rest/pastry/{pastryId} (GET) (Permission None)
```
{
    "pastryId":1,
    "name":"Rohlík",
    "cost":1.5,
    "category":
        {
            "categoryId": 1,
            "name": "Slanné pečivo"
        },
    "materials":
        [
            {
                "materialId":1,
                "name":"Hladká mouka",
                "amount":10.0
            },
            ...
        ]
}
```
3. rest/pastry (POST) (Permission manager)
```
{
    "name":"Chléb",
    "cost":25.9,
    "category":
        {
            "categoryId": 3
        },
    "materials":
        [
            {
                "materialId":1,
                "amount":100.0
            },
            ...
        ]
}
{
    "pastryId":2,
    "name":"Chléb",
    "cost":25.9,
    "category":
        {
            "categoryId": 3,
            "name": "Chleby"
        },
    "materials":
        [
            {
                "materialId":1,
                "name":"Hladká mouka",
                "amount":100.0
            },
            ...
        ]
}
```
4. rest/pastry/{pastryId} (PUT) Request payload == Response payload (Permission manager)
```
{
    "pastryId":2,
    "name":"Chléb",
    "cost":29.9,
    "category":
        {
            "categoryId": 3,
            "name": "Chleby"
        },
    "materials":
        [
            {
                "materialId":1,
                "name":"Hladká mouka",
                "amount":100.0
            },
            ...
        ]
}
```
6. rest/pastry/category/{categoryId} (GET) (return all pastry belong to defined category) (Permission None)
```
{
    "pastryId":1,
    "name":"Rohlík",
    "cost":1.5,
    "category":
        {
            "categoryId": 1,
            "name": "Slanné pečivo"
        },
    "materials":
        [
            {
                "materialId":1,
                "name":"Hladká mouka",
                "amount":10.0
            },
            ...
        ]
}
```
5. rest/pastry/{pastryId} (DELETE) (Permission manager)
## Material Storage
1. rest/materialstorage/list (GET) (Permission baker)
```
[
    {
        "amount":100.0,
        "material":
            {
                "materialId":1,
                "name":"Hladká mouka"
            }
    },
    ...
]
```
2. rest/materialstorage/enter (POST) (Permission materialstoreman)
```
{
    "materialId":1,
    "amount": 100.0
}
```
3. rest/materialstorage/enter/list (POST) (Permission materialstoreman)
```
[
    {
        "materialId":1,
        "amount": 100.0
    },
    ...
]
```
4. rest/materialstorage/withdraw (POST) (Permission materialstoreman)
```
{
    "materialId":1,
    "amount": 100.0
}
```
5. rest/materialstorage/withdraw/list (POST) (Permission materialstoreman)
```
[
    {
        "materialId":1,
        "amount": 100.0
    },
    ...
]
```
## Pastry Storage
1. rest/pastrystorage/list (GET) (Permission baker)
```
[
    {
        "count":50,
        "pastry":
            {
                "pastryId":1,
                "name":"Rohlík",
                "cost":1.5
            }

    },
    ...
]
```
2. rest/pastrystorage/enter (POST) (Permission pastrystoreman)
```
{
    "pastryId":1,
    "count": 50
}
```
3. rest/pastrystorage/enter/list (POST) (Permission pastrystoreman)
```
[
    {
        "pastryId":1,
        "count": 50
    },
    ...
]
```
4. rest/pastrystorage/withdraw (POST) (Permission pastrystoreman)
```
{
    "pastryId":1,
    "count": 50
}
```
5. rest/pastrystorage/withdraw/list (POST) (Permission pastrystoreman)
```
[
    {
        "pastryId":1,
        "count": 50
    },
    ...
]
```
## Orders
1. rest/orders/list (GET) (Permission pastrystoreman)
2. rest/orders/state/{state} (GET) (Permission pastrystoreman - return all orders in defined state, permission customer - return all customer's orders in defined state)
3. rest/orders/deliveryDate/{date} (GET) (date format yyyy-MM-dd) (Permission pastrystoreman)
4. rest/orders/customer/{userId} (GET) (Permission customer)

```
[
    {
        "orderId":1,
        "price":299.9,
        "state":"created",
        "deliveryDate":"2020-05-10",
        "customer":
            {
                "userId":1,
                "name":"Zakaznik",
                "surname":"Zakaznik",
                "email":"customer@gmail.com"
            },
        "items":
            [
                {
                    "pastryId":1,
                    "name":"Rohlík",
                    "count":5,
                    "cost":7.5
                },
                ...
            ]
    }
    ...
]
```
5. rest/orders/{orderId} (GET) (Permission pastrystoreman)
```
{
    "orderId":1,
    "price":299.9,
    "state":"created",
    "deliveryDate":"2020-05-10",
    "customer":
        {
            "userId":1,
            "name":"Zakaznik",
            "surname":"Zakaznik",
            "email":"customer@gmail.com"
        },
    "items":
        [
            {
                "pastryId":1,
                "name":"Rohlík",
                "count":5,
                "cost":7.5
            },
            ...
        ]
}
```
6. rest/orders (POST) (Permission customer)
```
{
    "state":"created",
    "deliveryDate":"2020-05-10",
    "customer":
        {
            "userId":1,
        },
    "items":
        [
            {
                "pastryId":1,
                "count":5
            },
            ...
        ]
}
```
7. rest/orders/{id} (PUT) (Permission pastrystoreman)
```
{
   "orderId":1,
    "state":"prepared",
    "deliveryDate":"2020-05-10",
    "customer":
        {
            "userId":1,
        },
    "items":
        [
            {
                "pastryId":1,
                "count":5
            },
            ...
        ]
}
```
8. rest/orders/cancel/{id} (PUT) (Permission customer)
```
{}
```
## Material Order
1. rest/materialorders/list (GET) (Permission materialstoreman)
2. rest/materialorders/delivered/{state} (state == true || false) (GET) (Permission materialstoreman)
```
[
    {
        "orderId":3,
        "dateOfCreation":"2020-02-11",
        "delivered":false,
        "materials":
            [
                {
                    "materialId":1,
                    "name":"Hladká mouka",
                    "cost":20.0,
                    "amount":500.0
                },
                ...
            ]
    },
    ...
]
```
3. rest/materialorders/{id} (GET) (Permission materialstoreman)
```
{
    "orderId":3,
    "dateOfCreation":"2020-02-11",
    "delivered":false,
    "materials":
        [
            {
                "materialId":1,
                "name":"Hladká mouka",
                "cost":20.0,
                "amount":500.0
            },
            ...
        ]
}
```
4. rest/materialorders (POST) (Permission manager)
```
{
    "dateOfCreation":"2020-02-11", (optional, in case of missing now is added)
    "materials":
        [
            {
                "materialId":1,
                "cost":20.0,
                "amount":500.0
            },
            ...
        ]
}
```
5. rest/materialorders/accept/{id} (PUT) (allows accept only nondelivered orders, and enters all items of order into material storage) (Permission materialstoreman)
5. rest/materialorders/cancel/{id} (DELETE) (allows cancel only nondelivered orders) (Permission manager)
## Production Plan
1. rest/productionPlans/list (GET) (Permission baker)
```
[
    {
        "planId":1,
        "accepted":false,
        "productionDate":"2020-05-10",
        "items":
            [
                {
                    "pastryId":1,
                    "name":"Rohlík",
                    "count":10
                },
                ...
            ],
        "materials":
            [
                {
                    "materialId":1,
                    "name":"Hladká mouka",
                    "amount":300.0,
                    "isEnough":false, (is send if productionDate is after today)
                    "diff":250.0 (is send if productionDate is after today)
                },
                ...
            ]
    },
    ...
]
```
2. rest/productionPlans/{id} (GET) (Permission baker)
3. rest/productionPlans/productionDate/{date} (GET) (date format yyyy-MM-dd) (Permission baker)
4. rest/productionPlans/create/{date} (GET) (date format yyyy-MM-dd) (Permission manager)
```
{
    "planId":1,
    "accepted":false,
    "productionDate":"2020-05-10",
    "items":
        [
            {
                "pastryId":1,
                "name":"Rohlík",
                "count":10
            },
            ...
        ],
    "materials":
        [
            {
                "materialId":1,
                "name":"Hladká mouka",
                "amount":300.0,
                "isEnough":false, (is send if productionDate is after today)
                "diff":250.0 (is send if productionDate is after today)
            },
            ...
        ]
}
```
5. rest/productionPlans/ (POST) (Permission manager)
```
{
    "accepted":false,
    "productionDate":"2020-05-10", (unique)
    "items":
        [
            {
                "pastryId":1,
                "count":20
            },
            ...
        ]
}
```
6. rest/productionPlans/{id} (PUT) (Permission manager)
```
{
    "planId":1,
    "accepted":true,
    "productionDate":"2020-05-10",
    "items":
        [
            {
                "pastryId":1,
                "count":20
            },
            ...
        ]
}
```
