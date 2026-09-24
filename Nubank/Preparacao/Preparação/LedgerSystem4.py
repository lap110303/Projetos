class LedgerSystem:
    def __init__(self):
        self.accounts = {}
        self.spent = {}
        self.transactions = {}
        self.disputed = set()
        self.tx_counter = 1
        self.history = []
        self.backups = {}
        self.backup_counter = 1

    def CreateAccount(self, timestamp: int, accountId: str) -> str:
        if accountId in self.accounts:
            return "false"

        self.accounts[accountId] = 0
        self.spent[accountId] = 0
        self.history.append(("CREATE", accountId))

        return "true"

    def Deposit(self, timestamp: int, accountId: str, amount : int) -> str:
        if accountId not in self.accounts:
            return ""

        self.accounts[accountId] += amount
        self.history.append(("DEPOSIT", accountId, amount))

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
        self.tx_counter += 1
        self.transactions[tx_id] = (sourceAccountId, targetAccountId, amount)
        self.history.append(("TRANSFER", tx_id, sourceAccountId, targetAccountId, amount))

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

        self.history.append(("DISPUTE", transactionId, src, tgt, amount))
        return "true"

    def Backup(self, timestamp: int) -> str:
        b_id = self.backup_counter
        self.backup_counter += 1
        self.backups[b_id] = len(self.history)

        return str(b_id)

    def Restore(self, timestamp: int, backupId: int) -> str:
        b_id = int(backupId)

        if b_id not in self.backups:
            return "false"

        target_len = self.backups[b_id]

        while self.history(len) > target_len:
            op = self.history.pop()
            op_type = op[0]

            if op_type == "CREATE":
                _, acc = op
                del self.accounts[acc]
                del self.spent[acc]

            elif op_type == "DEPOSIT":
                _, acc, amount = op
                self.accounts -= amount

            elif op_type == "TRANSFER":
                _, tx_id, src, tgt, amount = op
                self.accounts[src] += amount
                self.accounts[tgt] -= amount
                self.spent[src] -= amount
                del self.transactions[tx_id]
                self.tx_counter -= 1

            elif op_type == "DISPUTE":
                _, tx_id, src, tgt, amount = op
                self.accounts[src] -= amount
                self.accounts[tgt] += amount
                self.spent[src] += amount
                del self.disputed[tx_id]

        return "true"

