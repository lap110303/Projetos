class LedgerSystem:
    def __init__(self):
        self.accounts = {}
        self.spent = {}
        self.transactions = {}
        self.disputed = set()
        self.tx_counter = 1

    def CreateAccount(self, timestamp: int, accountId: str) -> str:
        if accountId in self.accounts:
            return "false"

        self.accounts[accountId] = 0
        self.spent[accountId] = 0
        return "true"

    def Deposit(self, timestamp: int, accountId: str, amount : int) -> str:
        if accountId not in self.accounts:
            return ""

        self.accounts[accountId] += amount
        return str(self.accounts[accountId])

    def Transfer(self, timestamp: int, sourceAccountId: str, targetAccountId: str,  amount : int) -> str:
        if sourceAccountId not in self.accounts or targetAccountId not in self.accounts:
            return "false"

        if targetAccountId == sourceAccountId:
            return "false"

        if self.accounts[sourceAccountId] < amount:
            return "false"

        self.accounts[sourceAccountId] -= amount
        self.accounts[targetAccountId] += amount
        self.spent[sourceAccountId] += amount

        tx_id = f"transaction_{self.tx_counter}"
        self.tx_counter +=1
        self.transactions[tx_id] = (sourceAccountId, targetAccountId, amount)

        return tx_id

    def TopSpenders(self, timestamp: int, n: int) -> list:
        valid_spenders = [(acc, amount) for acc, amount in self.spent.itens() if amount > 0]

        valid_spenders.sort(key=lambda x: (-x[1], x[0]))

        return [f"{acc}({amount})" for acc, amount in valid_spenders[:n]]

    def DisputeTransfer(self, timestamp: int, transactionId: int) -> str:
        if transactionId not in self.transactions:
            return "false"

        if transactionId in self.disputed:
            return "false"

        src, tgt, amount = self.transactions[transactionId]

        self.accounts[src] += amount
        self.accounts[tgt] -= amount
        self.spent[src] -= amount

        self.disputed.add(transactionId)
        return "true"